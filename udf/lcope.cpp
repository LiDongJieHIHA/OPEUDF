#include "Node.h"
#include <mysql.h>
#include <string.h>

std::map<string, long long> update;
Node *root = nullptr;
long long  start_update = -1;
long long end_update = -1;
extern "C" {
    //typedef long long longlong;
    //插入
    my_bool FHInsert_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
    long long FHInsert(UDF_INIT *initid, UDF_ARGS *args,char *is_null, char *error);
    //搜索
    my_bool FHSearch_init(UDF_INIT *const initid, UDF_ARGS *const args,char *const message);
    long long FHSearch(UDF_INIT *const initid, UDF_ARGS *const args,
                         char *const result, unsigned long *const length,
                         char *const is_null, char *const error);
    
    //更新
    my_bool FHUpdate_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
    long long FHUpdate(UDF_INIT *initid, UDF_ARGS *args,char *is_null, char *error);
    //更新范围
    my_bool FHStart_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
    long long FHStart(UDF_INIT *initid, UDF_ARGS *args,char *is_null, char *error);
    my_bool FHEnd_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
    long long FHEnd(UDF_INIT *initid, UDF_ARGS *args,char *is_null, char *error);
}

static char *
getba(UDF_ARGS *const args, int i, double&len)
{
    len = args->lengths[i];
    return args->args[i];
}


/*插入*/
long long FHInsert(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    int pos=*(int*)(args->args[0]);
    double keyLen;
    char *const keyBytes = getba(args, 1, keyLen);
    const std::string cipher = std::string(keyBytes, keyLen);
    // FILE * fp = fopen("logfh.txt", "a+");
    // fprintf(fp, "insert: ");
    // fprintf(fp, "%d", pos);
    // fprintf(fp, " ");
    // fprintf(fp, keyBytes);
    // fprintf(fp, " updatesize: ");
    // fclose(fp);
    long long start_update = -1;
    long long end_update = -1;
    update.clear();
    long long re = root->insert(pos, cipher);
    // fp = fopen("logfh.txt", "a+");
    // fprintf(fp, "%ld", update.size());
    // fprintf(fp, " result: ");
    // fprintf(fp, "%ld", re);
    // fprintf(fp, "\n");
    // fclose(fp);
    return re;
    //return pos;
}

my_bool FHInsert_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    start_update = -1;
    end_update = -1;
    update.clear();
    if (root == nullptr) {
        root_initial();
    }
    return 0;
}



/*搜索*/
long long
FHSearch(UDF_INIT *const initid, UDF_ARGS *const args,
                         char *const result, unsigned long *const length,
                         char *const is_null, char *const error)
{
    
    int pos=*(int*)(args->args[0]);
    if(pos == -1) return 0;
    return root->search(pos);
    // root->traverse();
    // return 0;
}
/*
my_bool FHSearch_init(UDF_INIT* initid, UDF_ARGS* args, char* message)
{
    return 0;
}
*/
my_bool
FHSearch_init(UDF_INIT *const initid, UDF_ARGS *const args,
                              char *const message)
{
    
    return 0;
}




long long FHUpdate(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    double keyLen;
    char *const keyBytes = getba(args, 0, keyLen);
    const std::string cipher = std::string(keyBytes, keyLen);
    // FILE * fp = fopen("logupdate.txt", "a+");
    // fprintf(fp, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
    // fprintf(fp, keyBytes);
    // fprintf(fp, "\n");
    // fclose(fp);
    // long long encoding = *(long long*)(args->args[1]);
    long long update_code = get_update(cipher);
    // fp = fopen("logupdate.txt", "a+");
    // fprintf(fp, "%ld", update_code);
    // fprintf(fp, "\n");
    // fprintf(fp, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
    // fclose(fp);
   
    return update_code;
    // return -2;
}

my_bool FHUpdate_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    return 0;
}



long long FHStart(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    return lower_bound();
}

my_bool FHStart_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    return 0;
}




long long FHEnd(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    
    return upper_bound();
}

my_bool FHEnd_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    return 0;
}
