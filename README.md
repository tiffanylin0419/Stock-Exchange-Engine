# Stock Exchange Engine

## Install

```
sudo apt install gcc
sudo apt install g++
sudo apt install make 
sudo apt install emacs
sudo apt install postgresql postgresql-contrib libpqxx-devmake
```
go to /etc/postgresql/12/main/
```
sudo emacs pg_hba.conf
```
Change line:
local all postgres peer
To:
local all postgres md5
```
sudo service postgresql restart
```

postgres command
sudo su - postgres
psql
ALTER USER postgres PASSWORD 'passw0rd';
CREATE DATABASE "EXCHANGE_SERVER";

## Tables
ACCOUNT: ACCOUNT_ID BALANCE

STOCK: STOCK_ID ACCOUNT_ID SYMBOL AMOUNT

ORDER: ORDER_ID ACCOUNT_ID SYMBOL AMOUNT PRICE STATE TIME

![IMG_3CD83214BC78-1](https://user-images.githubusercontent.com/88768257/227740167-b3db15d7-3f66-430e-a60c-b92877c4c058.jpeg)
