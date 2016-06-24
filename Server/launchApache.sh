#!/bin/bash

SF="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
IMAGE=morten/toptal


if [[ `docker ps | grep $IMAGE | wc -l` -ne "0" ]]
then    
    echo "There are already server instances running..."
    exit
fi

#Launch the development container
docker build -t $IMAGE $SF
  
     docker  run \
     --volume $SF/srv/www/:/var/www/ \
     --volume $SF/srv/etc/apache2:/etc/apache2 \
     --volume $SF/srv/logs/:/var/log/apache2 \
     -d \
     --link toptal-db:toptal-db \
     -p 443:443 \
     -e "SQL_DB_IP=toptal-db" \
     $IMAGE \
     /usr/sbin/apache2ctl -D FOREGROUND

     
 #Launch the unit test container
 docker  run \
     --volume $SF/srv/www/:/var/www/ \
     --volume $SF/srv/etc/apache2:/etc/apache2 \
     --volume $SF/tests/logs/apache:/var/log/apache2 \
     -d \
     --link toptal-db:toptal-db \
     -p 444:443 \
     -e "SQL_DB_IP=toptal-db" \
     -e "SQL_DB_USER"=testrunner \
     -e "SQL_DB_PWD"=testsecret \
     -e "SQL_DB_DB"=testing \
     -e "SQL_DB_PORT"=3306 \
     $IMAGE \
     /usr/sbin/apache2ctl -D FOREGROUND 
 
