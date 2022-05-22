import sys
sys.path.append('..')
from clientSide.client import OpeClient
import pymysql
from data.getData import getData
import datetime
import random

datapath = "data/"
logpath = "log/lc/"


def get_data(name):
    file = open(name)
    re = []
    for row in file:
        item_data = row.replace("\n", "").strip()
        re.append(item_data)
    # random.shuffle(re)
    return re

def getSearch(name):
    file = open(name)
    for row in file:
        re=row.replace("\n","")
        boundary=re.split("&")
        left = min(boundary[0].strip(), boundary[1].strip())
        right = max(boundary[0].strip(), boundary[1].strip())
        yield (left,right)
 
def assert_right(key1, key2, numlist):
    for key in numlist:
        if int(key) < int(key1) or int(key) > int(key2):
            return False
    return True

def initial_ciphertext(cursor):
     # initialize database
    sql = """
    drop table if exists example;
    """
    cursor.execute(sql)

    sql = """
    create table example (encoding bigint, ciphertext varchar(512));
    """
    cursor.execute(sql)

    #####################################
    sql = """
    drop procedure if exists pro_insert;
    """
    cursor.execute(sql)
    sql = """
    drop function if exists FHInsert;
    """
    cursor.execute(sql)

    sql = """
    drop function if exists FHSearch;
    """
    cursor.execute(sql)

    sql = """
    drop function if exists FHUpdate;
    """
    cursor.execute(sql)

    sql = """
    drop function if exists FHStart;
    """
    cursor.execute(sql)

    sql = """
    drop function if exists FHEnd;
    """
    cursor.execute(sql)

    #####################################
    sql = """
    create function FHInsert RETURNS INTEGER SONAME 'ope.so';
    """
    cursor.execute(sql)
    
    sql = """
    create function FHSearch RETURNS INTEGER SONAME 'ope.so';
    """
    cursor.execute(sql)

    sql = """
    create function FHUpdate RETURNS INTEGER SONAME 'ope.so';
    """
    cursor.execute(sql)

    sql = """
    create function FHStart RETURNS INTEGER  SONAME 'ope.so';
    """
    cursor.execute(sql)

    sql = """
    create function FHEnd RETURNS INTEGER SONAME 'ope.so';
    """
    cursor.execute(sql)

    sql = """
    create procedure pro_insert(IN pos int, IN ct varchar(512)) BEGIN DECLARE i BIGINT default 0; SET i = (FHInsert(pos,ct));  insert into example values (i,ct); if i = 0 then update example set encoding = FHUpdate(ciphertext) where (encoding >= FHStart() and encoding < FHEnd()) or (encoding = 0); end if;END
    """
    cursor.execute(sql)

    #####################################

def insert_ciphertext():
    opec = OpeClient(None)
    
    db = pymysql.connect(host="", user="",database="", password="", database="", password="", ssl={'ssl':{}})
    cursor = db.cursor()
    datasets = ["state"]
    grows = ["shuffle"]
    numberList=["other"]
    for number in numberList:
        for dataset in datasets:
            for grow in grows:
                initial_ciphertext(cursor)
                attr = dataset + "/" + grow + "/"
                log_file = open(logpath + "logUDFinsert.txt", 'a+')
                print("now we are starting "+attr+" "+str(number))
                item = attr + str(number)
                ###insert
                start_time = datetime.datetime.now()
                i = 0
                for index, insertkey in enumerate(get_data(datapath+item+ ".txt")):
                    i = i+1
                    pos, ciphertext = opec.insert_udf(insertkey)
                    # print(str(pos)+" "+ciphertext)
                    sql = "call pro_insert(%d,\"%s\");" %(pos, str(ciphertext))
                    cursor.execute(sql)
                    if i == 100 or i == 1000 or i == 10000 or i == 100000 or i == 1000000:
                        print("inserted data "+ str(i))
                        now_time = datetime.datetime.now()
                        log_file.write(attr+" "+str(number)+" "+str(i)+" "+str((now_time - start_time).total_seconds()))
                        print(attr+" "+str(number)+" "+str(i)+" "+str((now_time - start_time).total_seconds()))
                        log_file.write("\n")
                        search_start_time = datetime.datetime.now()
                        for index1, (left, right) in enumerate(getSearch(datapath+dataset + "/" +"search/"+str(number)+ ".txt")):
                            sql = "select ciphertext from example where encoding > FHSearch(%d) and encoding < FHSearch(%d);" %(opec.Search(left), opec.Search_right(right))
                            # print(sql)
                            cursor.execute(sql)
                            results = cursor.fetchall() 
                            for result in results:
                                plain_re = opec.decode(result[0]).split("#")[0]
                                j = left <= plain_re <= right
                                if not j :
                                    print(left)
                                    print(plain_re)
                                    print(right)
                                assert j
                            if index1 == 99:
                                print("search data "+ str(i))
                                search_now_time = datetime.datetime.now()
                                log_file.write("search "+ attr+" "+str(number)+" "+str(i)+" "+str((search_now_time - search_start_time).total_seconds()))
                                log_file.write("\n")
                                break

                now_time = datetime.datetime.now()
                log_file.write(attr+" "+str(number)+" "+str(i)+" "+str((now_time - start_time).total_seconds()))
                log_file.write("\n")
                sql = "select count(distinct encoding) from example" 
                cursor.execute(sql)
                results = cursor.fetchall() 
                for result in results:
                    print(result)
                
                # sql = "select distinct encoding from example" 
                # cursor.execute(sql)
                # results = cursor.fetchall() 
                # for result in results:
                #     print(result)
               
                # results = cursor.fetchall() 
                # for result in results:
                #     print(result)
                
    log_file.close()
    db.close()
        
def initial_plaintext(cursor):
    sql = """
    drop table  if exists plaintext;
    """
    cursor.execute(sql)

    sql = """
    create table plaintext (pt varchar(128));
    """
    cursor.execute(sql)

def insert_plaintext():
    
    opec = OpeClient(None)
    
    db = pymysql.connect(host="", user="",database="", password="",port=)
    cursor = db.cursor()
   
    attrList = ["firstname", "lastname"]
    for num in [1000000]:
        for attr in attrList:
            initial_plaintext(cursor)
            print("attr " + attr)
            log_file = open(logpath + "log_plaintext.txt", 'a+')
            start_insert = datetime.datetime.now()
            i = 0
            for insertkey in getData(datapath + attr+ str(num) + ".txt"):
                i = i+1
                # print(i)
                # pos, ciphertext = opec.insert(insertkey)
                insertkey = insertkey.replace("'"," ")
                sql = "insert into plaintext values ('%s')" %(insertkey)
                cursor.execute(sql)
                if i == 100 or i == 1000 or i == 10000 or i == 100000 or i == 1000000 or i == 10000000:
                    print(i)
                    end_insert = datetime.datetime.now()
                    log_file.write(str(i) + " " +attr+" "+ str((end_insert - start_insert).total_seconds()) + "\n")
    log_file.close()
    db.close()
        


if __name__ == '__main__':
    #insert_plaintext()
    insert_ciphertext()
