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

### connect every time
```
sudo su - postgres
psql
\c EXCHANGE_SERVER
SELECT * FROM STOCK;
```

## Tables
ACCOUNT: ACCOUNT_ID BALANCE

STOCK: STOCK_ID ACCOUNT_ID SYMBOL AMOUNT

ORDER: ORDER_ID ACCOUNT_ID SYMBOL AMOUNT PRICE STATE TIME

[table design](https://www.canva.com/design/DAFeQD_kGoI/zLW_Pd8YsPNxIJqVO13V2g/view?utm_content=DAFeQD_kGoI&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)


<img width="826" alt="截圖 2023-03-27 上午12 28 08" src="https://user-images.githubusercontent.com/88768257/227841147-9a061b8f-9f73-4119-82a8-47d70002d018.png">


