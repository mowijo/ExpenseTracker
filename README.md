This repository holds my solution to an assignment I once got:


> **Write a simple expenses tracker application**
> Write it in two parts: a client application and a server process
> The user must be able to create an account and log in.
> When logged in, user can see, edit and delete expenses he entered.
> When entered, each expense has: date, time, description, amount, comment
> Implement at least two roles with different permission levels (ie: a regular 
> user would only be able to CRUD on his owned records, a user manager would be 
> able to CRUD users, an admin would be able to CRUD on all records and users, 
> etc.)
> User can filter expenses.
> User can print expenses per week with total amount and average day spending.
> Minimal UI/UX design is needed.
> Server API. Make it possible to perform all user actions via the API, including 
> authentication. You need to be able to pass credentials to both the client and 
> the API.
> In any case you should be able to explain how a API works and demonstrate that 
> by creating functional tests that use the Layer directly.


Features
========

When logging on the client there are three roles: root, managers (that may approve expenses) and users that can create expenses.

The fixed account with email `root` with password `foobar` can user manage users and assign and revoke privileges. This user cannot approve expenses.

Users can either approve expenses or not. If they can approve expenses, they cannot create any them self.

Anybody can create an account and the user will be automatically logged in. Note the password that new users are assigned as it is not possible to change password.


Implementation Overview
=======================

The solution is a two part. A Server using a standard LAMP stack and a client written in C++ using the Qt5 libraries. You will need a Linux machine to run the server, while you can build and run the client on any platform you have a C++ compiler and the Qt libraries for.

The Server
----------
The server is implemented as a REST service implemented in PHP served by Apache providing JSON over HTTPS. The server provides four endpoints
```
/users/
/users/{id}
/expenses/
/expenses/{id}
```
and every request to any endpoint must contains proper credentials as HTTP Basic Auth. The only exception from credentials is the creation of users which can be done without credentials. The users and expenses are stored in a MySql database.

The Apache server and the mysql server runs in each their own docker container. The MySql docker images is grabbed from the Docker hub and the Apache image is build locally on a debian base.

There are scripts in the `Server` folder to launch the mysql container and the apache container, but they do not store any data persistently on the host and


<div style="border:2px solid #ff0000; background-color: #ffff00; font-weight: bold; text-align: center;border-radius: 1em; padding: 1em;">all data are lost when the MySQL container is shut down.
</div>

When you launch it again, it is populated with some default data.

The mysql container provides two identical databases for holding data. The one database is used for the productoin server, and the other is used as backend for automated tests and as development sandbox. To access the different datavases, there are scripts to launch a production container serving the REST server and a test/sandbox server serving the REST server. The production server can be reached via HTTPS on port 444 and the test/sandbox can be reached using HTTPS over port 444. The certificate is self signed and will raise a warning in our http client.

Both REST servers mounts the `Server/srv/www/sync` folder so any changes to the PHP sources in there will take effect immidately in both REST servers.

Launcing The Server
-------------------
To launch the server solution, you shold first launch the MySQL container and then the REST servers. Depending on how your system is set up, you may need to do this with root privileges.

  1. Run `Server/launchMysql.sh`. This will launch the MySql server and will generate an array of output that may look scary [^1]. Those are errors from setting up the databases and populating them and it is expected. Depending on how your system is set up, you may need to do this with root privileges.

  2. Run `Server/launchApache.sh`. This will launch and bind the REST servers to port 443 and port 444 on your host.

If you want to do a quick test to see if things are working, you can do that with curl:

```curl -k -H "Host: sync.data" -u foo@bar.com:foobar https://127.0.0.1/expenses/26```

this should return a JSON string representing the expense with id 26 from the prepopulated database.


Testing The Server
------------------

In the folder ```Server/tests/scripts``` are some unit tests and some functional tests that tests the logic and functionality of the server. 

Having launched the servers, you can go to ```Server/tests/scripts``` and do ```phpunit .``` and you will run the tests.

The Client
----------

The client is implemented in C++11 using th [Qt libraries](http://qt.io) and whild I have only tried it on Linux, it should be possible to build and execute it on Windows and MacOS as well.


After having [set up your host](#abcde) so you can build. Then go to the `Client/` folder and run 
```
qmake
make -j
```
After successful compilation, you can run 
```
Client/src/application/client
```

Setting Up Your Linux Host 
==========================
<a name="abcde"></a>
Assuming you have a Ubuntu 16.04 installation these steps will enable you to build and run the solution found in this repository. Make sure you have an internet connection and a private ssh key in ```~/.ssh``` that matches your public key on TopTals gitlab server, the do the following:

```
sudo apt-get install git qt5-default docker.io curl phpunit build-essential mysql-client php-mysql php-curl
cd ~
git clone git@git.toptal.com:Morten-Winkler-Jorgensen/morten-winkler-jorgensen.git
```


Known Bugs and Defects
======================

  1. If a user tries to change privileges for him self, the result is pretty non-deterministic. This does not seem like a dealbreaker for this assignment, so I have decided to leave it.
  
  2. Althoug the user interface is prepared for it, the server does not support deleting users.




[^1]: ```
Warning: Using a password on the command line interface can be insecure.
ERROR 2013 (HY000): Lost connection to MySQL server at 'reading initial communication packet', system error: 0
...
ERROR 2013 (HY000): Lost connection to MySQL server at 'reading initial communication packet', system error: 0
Warning: Using a password on the command line interface can be insecure.
...
Warning: Using a password on the command line interface can be insecure.
```

  
  