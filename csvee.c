#include <stdio.h>
#include <stdlib.h>

#include "csvee.h"

#define FIELD_BUF_SIZE 300

CSVParser * csv_new(char *file, int fields_per_line) {
    int i;

    CSVParser *new_parser = malloc(sizeof(CSVParser));
    new_parser->csv_file = fopen(file, "r");
    new_parser->fields_per_line = fields_per_line;

    new_parser->fields = malloc(sizeof(*new_parser->fields) * fields_per_line);
    for (i = 0; i < fields_per_line; i++) {
        new_parser->fields[i] = malloc(sizeof(**new_parser->fields) * FIELD_BUF_SIZE + 1);
    }

    return new_parser;
}

void csv_free(CSVParser *parser) {
    int i;

    for (i = 0; i < parser->fields_per_line; i++) {
        free(parser->fields[i]);
    }
    free(parser->fields);
    free(parser);
} 

char csv_parse(CSVParser *parser) {
    // char (int because of EOF) for read file loop
    int c;
    // iterator for buffers
    int i = 0, j = 0;
    // flag for comma-escaping backslash
    int found_backslash = 0;

    while ((c = fgetc(parser->csv_file)) != '\n' && c != EOF) {
        // if backslash is seen in a field, it's not written to the buffer
        // and the next character is treated as having no special function
        if (c == '\\') {
            found_backslash = 1;
        }
        
        else {
            if (c == ',' && !found_backslash) {
                // close current field and move to next
                parser->fields[i++][j] = '\0';
                // reset field itr
                j = 0;
            }

            else {
                parser->fields[i][j++] = c;
                // reset flag after finding a backslash
                found_backslash = 0;
            }
        }
    }

    // close last field
    parser->fields[i][j] = '\0';

    if (c == EOF) {
        fclose(parser->csv_file);
        csv_free(parser);
    }

    return c;
}

char * csv_get_field(CSVParser *parser, int field) {
    return parser->fields[field];
}
