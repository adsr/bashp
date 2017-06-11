#include "../parson/parson.h"
#include "../parson/parson.c"

JSON_Value *bashp_command_to_json(COMMAND *command);

bashp_json_object_set_string_or_null(JSON_Object *obj, const char *key, char *val) {
    if (val) {
        json_object_set_string(obj, key, val);
    } else {
        json_object_set_value(obj, key, json_value_init_null());
    }
}

JSON_Value *bashp_word_list_to_json(WORD_LIST *list) {
    JSON_Value *listval;
    JSON_Array *listarr;

    listval = json_value_init_array();
    listarr = json_value_get_array(listval);
    while (list) {
        json_array_append_string(listarr, list->word->word);
        list = list->next;
    }
    return listval;
}

void bashp_command_to_json_for(JSON_Object *obj, FOR_COM *c) {
    WORD_LIST *list;
    JSON_Value *listval;
    JSON_Array *listarr;
    JSON_Object *cmd;

    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    bashp_json_object_set_string_or_null(obj, "name", c->name ? c->name->word : NULL);
    json_object_set_value(obj, "map_list", bashp_word_list_to_json(c->map_list));
    json_object_set_value(obj, "action", bashp_command_to_json(c->action));
}

void bashp_command_to_json_case(JSON_Object *obj, CASE_COM *c) {
    PATTERN_LIST *clause;
    JSON_Value *clauseval;
    JSON_Object *clauseobj;
    JSON_Value *clausesval;
    JSON_Array *clausesarr;

    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    bashp_json_object_set_string_or_null(obj, "word", c->word ? c->word->word : NULL);
    clause = c->clauses;
    clausesval = json_value_init_array();
    clausesarr = json_value_get_array(clausesval);
    while (clause) {
        clauseval = json_value_init_object();
        clauseobj = json_value_get_object(clauseval);
        json_object_set_number(clauseobj, "flags", (double)clause->flags);
        json_object_set_value(clauseobj, "patterns", bashp_word_list_to_json(clause->patterns));
        json_object_set_value(clauseobj, "action", bashp_command_to_json(clause->action));
        json_array_append_value(clausesarr, clauseval);
        clause = clause->next;
    }
    json_object_set_value(obj, "clauses", clausesval);
}

void bashp_command_to_json_while(JSON_Object *obj, WHILE_COM *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_value(obj, "test", bashp_command_to_json(c->test));
    json_object_set_value(obj, "action", bashp_command_to_json(c->action));
}

void bashp_command_to_json_until(JSON_Object *obj, WHILE_COM *c) {
    bashp_command_to_json_while(obj, c);
}

void bashp_command_to_json_if(JSON_Object *obj, IF_COM *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_value(obj, "test", bashp_command_to_json(c->test));
    json_object_set_value(obj, "true_case", bashp_command_to_json(c->true_case));
    json_object_set_value(obj, "false_case", bashp_command_to_json(c->false_case));
}

void bashp_command_to_json_simple(JSON_Object *obj, SIMPLE_COM *c) {
    REDIRECT *redir;
    JSON_Value *redirval;
    JSON_Object *redirobj;
    JSON_Value *redirsval;
    JSON_Array *redirsarr;
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    json_object_set_value(obj, "words", bashp_word_list_to_json(c->words));
    redir = c->redirects;
    redirsval = json_value_init_array();
    redirsarr = json_value_get_array(redirsval);
    while (redir) {
        redirval = json_value_init_object();
        redirobj = json_value_get_object(redirval);
        // TODO Not sure if these are right
        if (redir->rflags & REDIR_VARASSIGN) {
            bashp_json_object_set_string_or_null(redirobj, "redirector", redir->redirector.filename ? redir->redirector.filename->word : NULL);
        } else {
            json_object_set_number(redirobj, "redirector", (double)redir->redirector.dest);
        }
        if (redir->instruction == r_duplicating_input || redir->instruction == r_duplicating_output) {
            json_object_set_number(redirobj, "redirectee", (double)redir->redirectee.dest);
        } else {
            bashp_json_object_set_string_or_null(redirobj, "redirectee", redir->redirectee.filename ? redir->redirectee.filename->word : NULL);
        }
        json_object_set_number(redirobj, "rflags", (double)redir->rflags);
        json_object_set_number(redirobj, "flags", (double)redir->flags);
        json_object_set_number(redirobj, "instruction", (double)redir->instruction);
        bashp_json_object_set_string_or_null(redirobj, "here_doc_eof", redir->here_doc_eof);
        json_array_append_value(redirsarr, redirval);
        redir = redir->next;
    }
    json_object_set_value(obj, "redirects", redirsval);
}

void bashp_command_to_json_select(JSON_Object *obj, SELECT_COM *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    bashp_json_object_set_string_or_null(obj, "name", c->name ? c->name->word : NULL);
    json_object_set_value(obj, "map_list", bashp_word_list_to_json(c->map_list));
    json_object_set_value(obj, "action", bashp_command_to_json(c->action));
}

void bashp_command_to_json_connection(JSON_Object *obj, CONNECTION *c) {
    json_object_set_value(obj, "first", bashp_command_to_json(c->first));
    json_object_set_value(obj, "second", bashp_command_to_json(c->second));
    json_object_set_number(obj, "connector", (double)c->connector);
}

void bashp_command_to_json_function_def(JSON_Object *obj, FUNCTION_DEF *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    bashp_json_object_set_string_or_null(obj, "name", c->name ? c->name->word : NULL);
    json_object_set_value(obj, "command", bashp_command_to_json(c->command));
    bashp_json_object_set_string_or_null(obj, "source_file", c->source_file);
}

void bashp_command_to_json_group(JSON_Object *obj, GROUP_COM *c) {
    json_object_set_value(obj, "command", bashp_command_to_json(c->command));
}

void bashp_command_to_json_arith(JSON_Object *obj, ARITH_COM *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    json_object_set_value(obj, "exp", bashp_word_list_to_json(c->exp));
}

void bashp_command_to_json_cond(JSON_Object *obj, COND_COM *c) {
    JSON_Value *leftval;
    JSON_Object *leftobj;
    JSON_Value *rightval;
    JSON_Object *rightobj;
    leftval = json_value_init_object();
    leftobj = json_value_get_object(leftval);
    rightval = json_value_init_object();
    rightobj = json_value_get_object(rightval);

    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    json_object_set_number(obj, "cond_type", (double)c->type);
    bashp_json_object_set_string_or_null(obj, "op", c->op ? c->op->word : NULL);
    if (c->left) bashp_command_to_json_cond(leftobj, c->left);
    if (c->right) bashp_command_to_json_cond(rightobj, c->right);
    json_object_set_value(obj, "left", leftval);
    json_object_set_value(obj, "right", rightval);
}

void bashp_command_to_json_arith_for(JSON_Object *obj, ARITH_FOR_COM *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_number(obj, "line", (double)c->line);
    json_object_set_value(obj, "init", bashp_word_list_to_json(c->init));
    json_object_set_value(obj, "test", bashp_word_list_to_json(c->test));
    json_object_set_value(obj, "step", bashp_word_list_to_json(c->step));
    json_object_set_value(obj, "action", bashp_command_to_json(c->action));
}

void bashp_command_to_json_subshell(JSON_Object *obj, SUBSHELL_COM *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_value(obj, "command", bashp_command_to_json(c->command));
}

void bashp_command_to_json_coproc(JSON_Object *obj, COPROC_COM *c) {
    json_object_set_number(obj, "flags", (double)c->flags);
    json_object_set_string(obj, "name", c->name);
    json_object_set_value(obj, "command", bashp_command_to_json(c->command));
}

JSON_Value *bashp_command_to_json(COMMAND *command) {
    JSON_Object *obj;
    JSON_Value *val;
    if (!command) {
        return json_value_init_null();
    }
    val = json_value_init_object();
    obj = json_value_get_object(val);
    switch (command->type) {
        #define case_x(px, pv) \
            case cm_ ## px : \
                json_object_set_string(obj, "type", #px); \
                bashp_command_to_json_ ## px (obj, command->value.pv); \
                break
        case_x(for, For);
        case_x(case, Case);
        case_x(while, While);
        case_x(until, While);
        case_x(if, If);
        case_x(simple, Simple);
        case_x(select, Select);
        case_x(connection, Connection);
        case_x(function_def, Function_def);
        case_x(group, Group);
        case_x(arith, Arith);
        case_x(cond, Cond);
        case_x(arith_for, ArithFor);
        case_x(subshell, Subshell);
        case_x(coproc, Coproc);
        default:
            json_object_set_null(obj, "type");
            break;
    }
    return val;
}

int
execute_command_internal (command, asynchronous, pipe_in, pipe_out, fds_to_close)
    COMMAND *command;
    int asynchronous;
    int pipe_in, pipe_out;
    struct fd_bitmap *fds_to_close;
{
    JSON_Value *val;
    char *json;
    val = bashp_command_to_json(command);
    json = json_serialize_to_string(val);
    printf("%s", json);
    json_free_serialized_string(json);
    json_value_free(val);
    return 0;
}
