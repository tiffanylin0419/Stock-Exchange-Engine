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

<img width="817" alt="截圖 2023-03-25 下午8 40 35" src="https://user-images.githubusercontent.com/88768257/227749100-2136f647-4d6a-453b-a017-4e594a97f52e.png">


