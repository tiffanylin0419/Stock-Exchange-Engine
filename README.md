# Stock Exchange Engine

## Install

```
sudo apt install gcc
sudo apt install g++
sudo apt install make 
sudo apt install emacs
sudo apt install postgresql postgresql-contrib libpqxx-devmake
```

postgres command
```
sudo su - postgres
psql
ALTER USER postgres PASSWORD 'passw0rd';
CREATE DATABASE "EXCHANGE_SERVER";
```

```
sudo emacs /etc/postgresql/12/main/pg_hba.conf
```
Change line:
local all postgres peer
To:
local all postgres md5
```
sudo service postgresql restart
```


## Tables
ACCOUNT: ACCOUNT_ID BALANCE

STOCK: STOCK_ID ACCOUNT_ID SYMBOL AMOUNT

ORDER: ORDER_ID ACCOUNT_ID SYMBOL AMOUNT PRICE STATE TIME

[table design](https://www.canva.com/design/DAFeQD_kGoI/zLW_Pd8YsPNxIJqVO13V2g/view?utm_content=DAFeQD_kGoI&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)
![IMG_3CD83214BC78-1](https://user-images.githubusercontent.com/88768257/227740167-b3db15d7-3f66-430e-a60c-b92877c4c058.jpeg)


