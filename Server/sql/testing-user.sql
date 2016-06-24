CREATE USER 'testrunner'@'%' IDENTIFIED BY 'testsecret';
GRANT ALL PRIVILEGES ON testing.* TO 'testrunner'@'%';
FLUSH PRIVILEGES;

