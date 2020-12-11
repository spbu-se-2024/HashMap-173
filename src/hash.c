#include "hash.h"

void free_words(char **words, int count_of_words) {
    for (int i = 0; i < count_of_words; i++) {
        free(words[i]);
    }
    free(words);
}

void hash_table_free(HASH_TABLE *hash_table, int table_size) {
    for (int i = 0; i < table_size; i++) {
        HASH_DATA *current_entry = hash_table->entries[i];
        HASH_DATA *next_entry;
        while (current_entry != NULL) {
            next_entry = current_entry->next;
            free(current_entry->key);
            free(current_entry);
            current_entry = next_entry;
        }
    }
    free(hash_table->entries);
    free(hash_table);
}

int common_hash(char *key, int table_size) {
    int hash_res = 0;
    int length = strlen(key);
    for (int i = 0; i < length; i++) {
        hash_res = ((hash_res + key[i]) * key[i]) % table_size;
    }
    return hash_res;
}

int polynomial_hash(char *key, int table_size) {
    int hash_res = 0;
    int length = strlen(key);
    for (int i = 0; i < length; i++) {
        hash_res = (97 * hash_res + key[i]) % table_size;
    }
    return hash_res;
}

long long int determine_file_size(FILE *file) {
    long long int size_file = 0;
    fseek(file, 0, SEEK_END);
    size_file = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size_file;
}

HASH_TABLE *hash_table_create(int table_size) {
    HASH_TABLE *hash_table = calloc(sizeof(HASH_TABLE), 1);
    if (!hash_table) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return NULL;
    }
    hash_table->entries = calloc(sizeof(HASH_DATA *), table_size);
    if (!hash_table->entries) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return NULL;
    }
    for (int i = 0; i < table_size; i++) {
        hash_table->entries[i] = NULL;
    }
    return hash_table;
}

HASH_DATA *hash_table_write_element(char *key) {
    HASH_DATA *hash_data = calloc(sizeof(HASH_DATA), 1);
    if (!hash_data) {
        fprintf(stderr, "Can not allocate memory\n");
        return NULL;
    }
    hash_data->key = calloc(strlen(key) + 1, 1);
    if (!hash_data->key) {
        fprintf(stderr, "Can not allocate memory\n");
        return NULL;
    }
    strcpy(hash_data->key, key);
    hash_data->next = NULL;
    return hash_data;
}

int hash_insert(HASH_TABLE *hash_table, char *key, int table_size, hash_func_ hash_func) {
    int slot = hash_func(key, table_size);
    HASH_DATA *current_entry = hash_table->entries[slot];
    if (current_entry == NULL) {
        if (!(hash_table->entries[slot] = hash_table_write_element(key))) {
            return -1;
        }
        hash_table->entries[slot]->count = 1;
        return 0;
    }
    HASH_DATA *previous_entry;
    while (current_entry != NULL) {
        if (strcmp(current_entry->key, key) == 0) {
            current_entry->count++;
            return 0;
        }
        previous_entry = current_entry;
        current_entry = previous_entry->next;
    }
    previous_entry->next = hash_table_write_element(key);
    return 0;
}

HASH_DATA *hash_find_most_common_word(HASH_TABLE *hash_table, int table_size) {
    int number_of_uses = 0;
    HASH_DATA *most_common_word;
    for (int i = 0; i < table_size; i++) {
        HASH_DATA *current_entry = hash_table->entries[i];
        HASH_DATA *next_entry;
        while (current_entry != NULL) {
            next_entry = current_entry->next;
            if (current_entry->count > number_of_uses) {
                most_common_word = current_entry;
                number_of_uses = current_entry->count;
            }
            current_entry = next_entry;
        }
    }
    return most_common_word;
}

char **read(char *filename, int *count_result_words) {
    FILE *input_file;
    if ((input_file = fopen(filename, "rb")) == NULL) {
        printf("Can not open file in.txt.\n");
        return NULL;
    }
    long long int file_size = determine_file_size(input_file);
    char *buffer;
    buffer = calloc(file_size, 1);
    if (!buffer) {
        fclose(input_file);
        fprintf(stderr, "Can not create buffer\n");
        return NULL;
    }
    fread(buffer, file_size, sizeof(char), input_file);
    fclose(input_file);
    char *buffer1;
    buffer1 = calloc(file_size, 1);
    if (!buffer1) {
        fprintf(stderr, "Can not create buffer\n");
        return NULL;
    }
    int count_of_recorded_chars = 0, count_of_words = 0, check_for_count_of_words = 0;
    for (int i = 0; i < file_size; i++) {
        if (!((buffer[i] >= '!' && buffer[i] <= '&') || (buffer[i] >= '(' && buffer[i] <= ',') ||
              (buffer[i] >= '.' && buffer[i] <= '@') || (buffer[i] >= '[' && buffer[i] <= '`') ||
              (buffer[i] >= '{' && buffer[i] <= '~'))) {
            buffer1[count_of_recorded_chars] = buffer[i];
            count_of_recorded_chars++;
            if (check_for_count_of_words == 0 && buffer[i] != ' ' && buffer[i] != '\n' &&
                buffer[i] != '\r' && buffer[i] != '\'' && buffer[i] != '-') {
                check_for_count_of_words = 1;
                count_of_words++;
            } else if (check_for_count_of_words == 1 && (buffer[i] == ' ' || buffer[i] == '\n' ||
                                                         buffer[i] == '\'' || buffer[i] == '-')) {
                check_for_count_of_words = 0;
            }
        }
    }
    char **words = calloc(sizeof(char *), count_of_words);
    if (!words) {
        fprintf(stderr, "Can not allocate memory\n");
        return NULL;
    }
    int char_number = 0;
    int word_number = -1;
    for (int i = 0; i < count_of_recorded_chars; i++) {
        if (char_number == 0 && buffer1[i] != ' ' && buffer1[i] != '\n' && buffer1[i] != '\r' && buffer1[i] != '\'' &&
            buffer1[i] != '-') {
            word_number++;
            words[word_number] = (char *) calloc(sizeof(char), MAX_LENGTH_WORD);
            words[word_number][char_number] = buffer1[i];
            char_number++;
        } else if (buffer1[i] == ' ' || buffer1[i] == '\n' ||
                   buffer1[i] == '\r' || buffer1[i] == '\'' || buffer1[i] == '-') {
            char_number = 0;
        } else {
            words[word_number][char_number] = buffer1[i];
            char_number++;
        }
    }
    free(buffer);
    free(buffer1);
    *count_result_words = count_of_words;
    return words;
}

void statistics_output(HASH_TABLE *hash_table, int table_size) {
    int count_of_keys = 0, count_of_chains = 0, length_of_chain = 0, max_length_of_chain = 0;
    for (int i = 0; i < table_size; i++) {
        HASH_DATA *current_entry = hash_table->entries[i];
        HASH_DATA *next_entry;
        if (current_entry != NULL) {
            count_of_keys++;
        }
        while (current_entry != NULL) {
            count_of_chains++;
            length_of_chain++;
            next_entry = current_entry->next;
            current_entry = next_entry;
        }
        if (max_length_of_chain < length_of_chain) {
            max_length_of_chain = length_of_chain;
        }
        length_of_chain = 0;
    }
    HASH_DATA *most_common_word;
    most_common_word = hash_find_most_common_word(hash_table, table_size);
    printf("Number of different words: %d\n", count_of_chains);
    printf("Most common word: '%s', count of uses: %d\n", most_common_word->key, most_common_word->count);
    printf("Total number of keys: %d\n", count_of_keys);
    printf("Max length of chain: %d\n", max_length_of_chain);
}

int hash_is_there(HASH_TABLE *hash_table, char *key, int table_size, hash_func_ hash_func) {
    int slot = hash_func(key, table_size);
    HASH_DATA *current_entry = hash_table->entries[slot];
    HASH_DATA *previous_entry;
    while (current_entry != NULL) {
        if (strcmp(current_entry->key, key) == 0) {
            return 1;
        }
        previous_entry = current_entry;
        current_entry = previous_entry->next;
    }
    return 0;
}

void print_error() {
    printf("Call function like this:\n");
    printf("1. Name of input file\n");
    printf("2. Size of hash table (must be number and more, then null)\n");
    printf("3. Hash function (common, polynomial, md5)\n");
    printf("4. Not compulsory! Enter the word, you want to find in the text\n");
}

int checking_arguments(char *argv[], int argc) {
    if (argc != 4 && argc != 5) {
        print_error();
        return -1;
    }
    int entered_table_size = atoi(argv[2]);
    if (entered_table_size <= 0) {
        print_error();
        return -1;
    }
    if (strncmp(argv[3], "common", 6) != 0 && strncmp(argv[3], "md5", 3) != 0 && strncmp(argv[3], "polynomial", 10) != 0) {
        print_error();
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (checking_arguments(argv, argc)) {
        return -1;
    }
    int table_size = atoi(argv[2]);
    hash_func_ hash_func;
    if (strncmp(argv[3], "common", 6) == 0) {
        hash_func = common_hash;
    }
    else if (strncmp(argv[3], "md5", 3) == 0) {
        hash_func = md5_hash;
    }
    else {
        hash_func = polynomial_hash;
    }
    char **array_of_words;
    int count_of_words;
    array_of_words = read(argv[1], &count_of_words);
    if (!array_of_words) {
        printf("Can not read file\n");
        return -1;
    }
    printf("Number of words: %d\n", count_of_words);
    HASH_TABLE *hash_table = hash_table_create(table_size);
    for (int i = 0; i < count_of_words; i++) {
        if (hash_insert(hash_table, array_of_words[i], table_size, hash_func)) {
            return -1;
        }
    }
    statistics_output(hash_table, table_size);
    if (argc == 5) {
        if (hash_is_there(hash_table, argv[4], table_size, hash_func) == 1) {
            printf("Word '%s' exists in the table\n", argv[3]);
        } else {
            printf("Word '%s' doesn't exist in the table\n", argv[4]);
        }
    }
    free_words(array_of_words, count_of_words);
    hash_table_free(hash_table, table_size);
    return 0;
}