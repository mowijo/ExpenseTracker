FROM debian                                                                                                        

MAINTAINER Morten Jorgensen

RUN   apt-get update               \
    &&                             \
      apt-get install -y           \
        apache2                    \
        php5                       \
        php5-mysqlnd               \
    &&                             \
      apt-get clean                \
    &&                             \
      rm -rf /var/lib/apt/lists/*  \
    &&                             \
      a2enmod ssl                  \
    &&                             \
      a2enmod rewrite              
      

ENV APACHE_RUN_USER www-data
ENV APACHE_RUN_GROUP www-data
ENV APACHE_LOG_DIR /var/log/apache2

EXPOSE 443

VOLUME /var/www /etc/apache2 /var/log/apache2 

