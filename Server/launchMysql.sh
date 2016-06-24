#!/bin/bash


if [[ `docker ps | grep toptal-db | wc -l` -ne "0" ]]
then    
    echo "There is already a MySQL instance running..."
    exit
fi

SF="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

docker rm toptal-db
docker run -e MYSQL_ROOT_PASSWORD=my-secret-pw -d -p 3306:3306 --name=toptal-db mysql/mysql-server 
mysql -h 127.0.0.1 -D mysql --user=root --port=3306 --password=my-secret-pw -e "create database data" 
while [[ "$?" -ne "0" ]]
do
    sleep 1
    mysql -h 127.0.0.1 -D mysql --user=root --port=3306 --password=my-secret-pw -e "create database data"
done
mysql -h 127.0.0.1 -D data --user=root --port=3306 --password=my-secret-pw < $SF/sql/schema.sql
mysql -h 127.0.0.1 -D data --user=root --port=3306 --password=my-secret-pw < $SF/sql/web-user.sql
mysql -h 127.0.0.1 -D data --user=root --port=3306 --password=my-secret-pw < $SF/sql/devbox-setup.sql

#
# Next, setup the database used for unittesting, integrationn testing and every other type of 
# test that needs a disposable sandbox database
# 
# The content of this database may be deleted frequently, so you'd better make sure yourself that 
# it contains the tables you need when you need them.
#
mysql -h 127.0.0.1 -D mysql --user=root --port=3306 --password=my-secret-pw -e "create database testing" 
mysql -h 127.0.0.1 -D testing --user=root --port=3306 --password=my-secret-pw < $SF/sql/schema.sql
mysql -h 127.0.0.1 -D testing --user=root --port=3306 --password=my-secret-pw < $SF/sql/testing-user.sql
mysql -h 127.0.0.1 -D testing --user=root --port=3306 --password=my-secret-pw < $SF/sql/devbox-setup.sql

