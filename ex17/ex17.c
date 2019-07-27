#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Address {
    int id;
    int set;
    char *name;
    char *email;
};

struct Database {
    int max_rows;
    int max_data;
    struct Address *rows;
};

struct Connection {
    FILE *file;
    struct Database *db;
};

void Database_close(struct Connection *conn);

int is_substring(const char *substring, const char *string)
{

    int substringlen = strlen(substring);
    int stringlen = strlen(string);

    for (int i = 0; i <= (stringlen - substringlen); i++)
    {
        int j;
        for (j = 0; j < substringlen; j++)
            if (string[i+j] != substring[j])
                break;
        
        if (j == substringlen)
            return 0;
    }
    return 1;
}

void die(const char *message, struct Connection *conn)
{
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    Database_close(conn);

    exit(1);
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn)
{
    if (fread(&conn->db->max_rows, sizeof(int), 1, conn->file) != 1)
        die("Couldn't read the max_rows", conn);

    if (fread(&conn->db->max_data, sizeof(int), 1, conn->file) != 1)
        die("Couldn't read the max_data", conn);

    int string_size = sizeof(char) * conn->db->max_data;
    int row_size = (sizeof(int) * 2) + (string_size * 2);

    conn->db->rows = malloc(row_size * conn->db->max_rows);
    if (!conn->db->rows)
        die("Memory error", conn);

    for(int i = 0; i < conn->db->max_rows; i++)
    {
        struct Address *addr = &conn->db->rows[i];

        if (fread(&addr->id, sizeof(addr->id), 1, conn->file) != 1)
            die("Couldn't read the id", conn);
        if (fread(&addr->set, sizeof(addr->set), 1, conn->file) != 1)
            die("Couldn't read the set", conn);

        addr->name = malloc(string_size);
        if (!addr->name)
            die("Memory error", conn);
        if (fread(addr->name, string_size, 1, conn->file) != 1)
            die("Couldn't read the name", conn);

        addr->email = malloc(string_size);
        if (!addr->email)
            die("Memory error", conn);
        if (fread(addr->email, string_size, 1, conn->file) != 1)
            die("Couldn't read the email", conn);
    }

}

void Database_close(struct Connection *conn)
{
    if (conn) {

        if (conn->file)
            fclose(conn->file);

        if (conn->db->rows) {
            for (int i = 0; i < conn->db->max_rows; i++)
            {
                if (conn->db->rows[i].name) free(conn->db->rows[i].name);
                if (conn->db->rows[i].email) free(conn->db->rows[i].email);
            }
            free(conn->db->rows);
        }
    }
}

void Database_write(struct Connection *conn)
{
    rewind(conn->file);

    int string_size = sizeof(char) * conn->db->max_data;

    if (fwrite(&conn->db->max_rows, sizeof(int), 1, conn->file) != 1)
        die("Failed to write the max_rows", conn);
    if (fwrite(&conn->db->max_data, sizeof(int), 1, conn->file) != 1)
        die("Failed to write the max_data", conn);

    for (int i = 0; i < conn->db->max_rows; i++)
    {
        if (fwrite(&conn->db->rows[i].id, sizeof(int), 1, conn->file) != 1)
            die("Failed to write the row->id", conn);
        if (fwrite(&conn->db->rows[i].set, sizeof(int), 1, conn->file) != 1)
            die("Failed to write the row->set", conn);
        if (fwrite(conn->db->rows[i].name, string_size, 1, conn->file) != 1)
            die("Failed to write the row->name", conn);
        if (fwrite(conn->db->rows[i].email, string_size, 1, conn->file) != 1)
            die("Failed to write the row->email", conn);
    }

    if (fflush(conn->file) == -1)
        die("Cannot flush database.", conn);
}

void Database_create(struct Connection *conn)
{
    int row_size = (sizeof(int) * 2) + (conn->db->max_data * sizeof(char) * 2);

    conn->db->rows = calloc(conn->db->max_rows, row_size);
    if (!conn->db->rows)
        die("Memory error", conn);

    for (int i = 0; i < conn->db->max_rows ; i++)
    {
        struct Address addr = {.id = i, .set = 0};
        addr.name = calloc(conn->db->max_data, sizeof(char));
        addr.email = calloc(conn->db->max_data, sizeof(char));
        conn->db->rows[i] = addr;
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{

    struct Address *addr = &conn->db->rows[id];

    if (addr->set)
        die("Already set, delete it first", conn);

    addr->set = 1;

    char *res = strncpy(addr->name, name, conn->db->max_data);
    if (!res)
        die("Name copy failed", conn);

    addr->name[(conn->db->max_data - 1)] = 0;

    res = strncpy(addr->email, email, conn->db->max_data);
    if (!res)
        die("Email copy failed", conn);

    addr->email[(conn->db->max_data - 1)] = 0;

}

void Database_find(struct Connection *conn, char *find)
{
    for (int i = 0; i < conn->db->max_rows; i++)
    {   
        if (conn->db->rows[i].set)
            if ((is_substring(find, conn->db->rows[i].name) == 0) ||
                (is_substring(find, conn->db->rows[i].email) == 0))
            {
                printf("Record found at ");
                Address_print(&conn->db->rows[i]);
            }
    }
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = &conn->db->rows[id];

    if (addr->set) {
        Address_print(addr);
    } else {
        die("ID is not set", conn);
    }
}

void Database_delete(struct Connection *conn, int id)
{
    conn->db->rows[id].set = 0;
}

void Database_list(struct Connection *conn)
{
    struct Database *db = conn->db;

    for (int i = 0; i < db->max_rows; i++)
    {
        struct Address *cur = &db->rows[i];
        if (cur->set) {
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        die("USAGE: ex17 <dbfile> <action> [action params]", NULL);

    char *filename = argv[1];
    char action = argv[2][0];

    struct Database actual_db;
    struct Connection actual_conn;

    struct Connection *conn = &actual_conn;
    conn->db = &actual_db;

    if (action == 'c') {
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+");
        if (conn->file) {
            Database_load(conn);
        }
    }

    if (!conn->file)
        die("Failed to open the file", conn);

    if (action != 'c' && action != 'f' &&
            argc > 3 && atoi(argv[3]) >= conn->db->max_rows)
        die("There aren't that many records", conn);

    switch (action) {
        case 'c':
            if (argc != 5)
                die("max_rows and max_data required", conn);
            conn->db->max_rows = atoi(argv[3]);
            conn->db->max_data = atoi(argv[4]);
            Database_create(conn);
            Database_write(conn);
            break;

        case 'g':
            if (argc != 4)
                die("Need an id to get", conn);

            Database_get(conn, atoi(argv[3]));
            break;

        case 's':
            if (argc != 6)
                die("Need id, name and email to set", conn);

            Database_set(conn, atoi(argv[3]), argv[4], argv[5]);
            Database_write(conn);
            break;

        case 'd':
            if (argc != 4)
                die("Need id to delete", conn);
            Database_delete(conn, atoi(argv[3]));
            Database_write(conn);
            break;

        case 'l':
            Database_list(conn);
            break;

        case 'f':
            if (argc != 4)
                die("Need the name/email", conn);
            Database_find(conn, argv[3]);
            break;

        default:
            die("Invalid action: c=create, g=get, s=set, d=del, l=list\n", conn);
    }

    Database_close(conn);

    return 0;
}
