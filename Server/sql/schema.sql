

drop table if exists Users;
create table Users (
    id int primary key not null auto_increment,
    email varchar(255) unique not null,
    name varchar(255),
    password varchar(255),
    privileges int
);


drop table if exists Expenses;
create table Expenses
(
    id int primary key not null auto_increment,
    owner int,
    epoch varchar(255),
    comment varchar(255),
    description varchar(255),
    amount int,
    deleted int,
    approved int,
    touched varchar(255)
);

insert into Users values(0, "root", "Administrator(pwd 'foobar')", "$2y$10$ucYtAvQkUQ6dgWQk3JwfZ.LRoctyKNF1hdUnkg/ZhQcPuQ2A6qqXW", 2);
