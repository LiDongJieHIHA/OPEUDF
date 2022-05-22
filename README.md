# OPEUDF

We have implemented frequency hiding order-preserving encryption schemes for MySQL.
These following features are included in our model:
UDF code for our scheme


###Hint: in order to work properly, you should compile `ope.cc` manually. Shell codes:
```sh
g++ -c -o ope.o -std=c++11 -I </path/to/your/mysql/include/lib> -fPIC -Wall ope.cc;
g++ -shared -o ope.so ope.o;
```
Then add `ope.so` to your MySQL plugin directory: `/usr/lib/mysql/plugins/*.so`.
```sh
sudo mv obj/ope.so /usr/lib/mysql/plugins;
```

and it will generate an `obj` directory in which you can find `ope.o` as well as `ope.so`.
Next, create all the functions with soname:
```sql
CREATE FUNCTION XXX RETURNS REAL SONAME 'ope.so';
```
step by step.
