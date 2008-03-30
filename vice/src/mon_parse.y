/* -*- C -*-
 *
 * mon_parse.y - Parser for the VICE built-in monitor.
 *
 * Written by
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

%{
#include "vice.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "asm.h"
#include "console.h"
#include "machine.h"
#include "mon.h"
#include "types.h"
#include "utils.h"

#define join_ints(x,y) (LO16_TO_HI16(x)|y)
#define separate_int1(x) (HI16_TO_LO16(x))
#define separate_int2(x) (LO16(x))

static int yyerror(char *s);
static int temp;
static int resolve_datatype(unsigned guess_type, char *num);

/* Defined in the lexer */
extern int new_cmd, opt_asm;
extern void free_buffer(void);
extern void make_buffer(char *str);
extern int yylex(void);
extern int cur_len, last_len;

#define ERR_ILLEGAL_INPUT 1     /* Generic error as returned by yacc.  */
#define ERR_RANGE_BAD_START 2
#define ERR_RANGE_BAD_END 3
#define ERR_BAD_CMD 4
#define ERR_EXPECT_BRKNUM 5
#define ERR_EXPECT_END_CMD 6
#define ERR_MISSING_CLOSE_PAREN 7
#define ERR_INCOMPLETE_COMPARE_OP 8
#define ERR_EXPECT_FILENAME 9
#define ERR_ADDR_TOO_BIG 10
#define ERR_IMM_TOO_BIG 11
#define ERR_EXPECT_STRING 12
#define ERR_UNDEFINED_LABEL 13

#define BAD_ADDR (new_addr(e_invalid_space, 0))
#define CHECK_ADDR(x) ((x) == LO16(x))

%}

%union {
	MON_ADDR a;
        int i;
        REG_ID reg;
        CONDITIONAL cond_op;
        CONDITIONAL_NODE *cond_node;
        RADIXTYPE rt;
        ACTION action;
        char *str;
}

%token<i> H_NUMBER D_NUMBER O_NUMBER B_NUMBER CONVERT_OP B_DATA
%token<str> D_NUMBER_GUESS O_NUMBER_GUESS B_NUMBER_GUESS
%token<i> TRAIL BAD_CMD MEM_OP IF MEM_COMP MEM_DISK8 MEM_DISK9 CMD_SEP REG_ASGN_SEP EQUALS
%token<i> CMD_SIDEFX CMD_RETURN CMD_BLOCK_READ CMD_BLOCK_WRITE CMD_UP CMD_DOWN
%token<i> CMD_LOAD CMD_SAVE CMD_VERIFY CMD_IGNORE CMD_HUNT CMD_FILL CMD_MOVE
%token<i> CMD_GOTO CMD_REGISTERS CMD_READSPACE CMD_WRITESPACE CMD_RADIX
%token<i> CMD_MEM_DISPLAY CMD_BREAK CMD_TRACE CMD_IO CMD_BRMON CMD_COMPARE
%token<i> CMD_DUMP CMD_UNDUMP CMD_EXIT CMD_DELETE CMD_CONDITION CMD_COMMAND
%token<i> CMD_ASSEMBLE CMD_DISASSEMBLE CMD_NEXT CMD_STEP CMD_PRINT CMD_DEVICE
%token<i> CMD_HELP CMD_WATCH CMD_DISK CMD_SYSTEM CMD_QUIT CMD_CHDIR CMD_BANK
%token<i> CMD_LOAD_LABELS CMD_SAVE_LABELS CMD_ADD_LABEL CMD_DEL_LABEL CMD_SHOW_LABELS
%token<i> CMD_RECORD CMD_STOP CMD_PLAYBACK CMD_CHAR_DISPLAY CMD_SPRITE_DISPLAY
%token<i> CMD_TEXT_DISPLAY CMD_ENTER_DATA CMD_ENTER_BIN_DATA
%token<i> CMD_BLOAD CMD_BSAVE CMD_SCREEN CMD_UNTIL
%token<i> L_PAREN R_PAREN ARG_IMMEDIATE REG_A REG_X REG_Y COMMA INST_SEP
%token<str> STRING FILENAME R_O_L OPCODE LABEL BANKNAME
%token<reg> REGISTER
%left<cond_op> COMPARE_OP
%token<rt> RADIX_TYPE INPUT_SPEC
%token<action> CMD_CHECKPT_ON CMD_CHECKPT_OFF TOGGLE

%type<a> address opt_address
%type<cond_node> cond_expr compare_operand
%type<i> command number expression d_number guess_default
%type<i> memspace memloc memaddr breakpt_num opt_mem_op
%type<i> register_mod opt_count command_list top_level value
%type<i> asm_operand_mode assembly_instruction end_cmd register
%type<i> assembly_instr_list post_assemble opt_memspace
%type<str> rest_of_line data_list data_element filename opt_bankname

%type<i> symbol_table_rules asm_rules memory_rules checkpoint_rules
%type<i> checkpoint_control_rules monitor_state_rules
%type<i> monitor_misc_rules disk_rules cmd_file_rules
%type<i> machine_state_rules data_entry_rules

%left '+' '-'
%left '*' '/'

%%

top_level: command_list { $$ = 0; }
         | assembly_instruction TRAIL { $$ = 0; }
         | TRAIL { new_cmd = 1; asm_mode = 0;  $$ = 0; }
         ;

command_list: command
            | command_list command
            ;

end_cmd: CMD_SEP
       | TRAIL
       | error { return ERR_EXPECT_END_CMD; }
       ;

command: machine_state_rules
       | symbol_table_rules
       | memory_rules
       | asm_rules
       | checkpoint_rules
       | checkpoint_control_rules
       | monitor_state_rules
       | monitor_misc_rules
       | disk_rules
       | cmd_file_rules
       | data_entry_rules
       | BAD_CMD { return ERR_BAD_CMD; }
       ;

machine_state_rules: CMD_BANK opt_memspace opt_bankname end_cmd { mon_bank($2,$3); }
                   | CMD_GOTO address end_cmd { mon_jump($2); }
                   | CMD_IO end_cmd { mon_display_io_regs(); }
                   | CMD_RETURN end_cmd { mon_instruction_return(); }
                   | CMD_DUMP filename end_cmd { machine_write_snapshot($2,0,0); /* FIXME */ }
                   | CMD_UNDUMP filename end_cmd { machine_read_snapshot($2); }
                   | CMD_STEP opt_count end_cmd { mon_instructions_step($2); }
                   | CMD_NEXT opt_count end_cmd { mon_instructions_next($2); }
                   | CMD_UP opt_count end_cmd { mon_stack_up($2); }
                   | CMD_DOWN opt_count end_cmd { mon_stack_down($2); }
                   | CMD_SCREEN end_cmd { mon_display_screen(); }
                   | register_mod
                   ;

register_mod: CMD_REGISTERS end_cmd		{ mon_print_registers(default_memspace); }
            | CMD_REGISTERS memspace end_cmd    { mon_print_registers($2); }
            | CMD_REGISTERS reg_list end_cmd
            ;

symbol_table_rules: CMD_LOAD_LABELS opt_memspace filename end_cmd 	{ playback = TRUE; playback_name = $3; /*mon_load_symbols($2, $3);*/ }
                  | CMD_SAVE_LABELS opt_memspace filename end_cmd 	{ mon_save_symbols($2, $3); }
                  | CMD_ADD_LABEL address LABEL end_cmd 		{ mon_add_name_to_symbol_table($2, $3); }
                  | CMD_DEL_LABEL opt_memspace LABEL end_cmd 		{ mon_remove_name_from_symbol_table($2, $3); }
                  | CMD_SHOW_LABELS opt_memspace end_cmd 		{ mon_print_symbol_table($2); }
                  ;

asm_rules: CMD_ASSEMBLE address { mon_start_assemble_mode($2, NULL); } post_assemble end_cmd
         | CMD_ASSEMBLE address end_cmd 		{ mon_start_assemble_mode($2, NULL); }
         | CMD_DISASSEMBLE address opt_address end_cmd  { mon_disassemble_lines($2,$3); }
         | CMD_DISASSEMBLE end_cmd 			{ mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
         ;

memory_rules: CMD_MOVE address address address end_cmd 		  { mon_move_memory($2, $3, $4); }
            | CMD_COMPARE address address address end_cmd 	  { mon_compare_memory($2, $3, $4); }
            | CMD_FILL address address data_list end_cmd 	  { mon_fill_memory($2,$3,(unsigned char *)$4); }
            | CMD_HUNT address address data_list end_cmd 	  { mon_hunt_memory($2,$3,(unsigned char *)$4); }
            | CMD_MEM_DISPLAY RADIX_TYPE address opt_address end_cmd { mon_display_memory($2, $3, $4); }
            | CMD_MEM_DISPLAY address opt_address end_cmd 	  { mon_display_memory(default_radix, $2, $3); }
            | CMD_MEM_DISPLAY end_cmd 				  { mon_display_memory(default_radix, BAD_ADDR, BAD_ADDR); }
            | CMD_CHAR_DISPLAY address opt_address end_cmd 	  { mon_display_data($2, $3, 8, 8); }
            | CMD_CHAR_DISPLAY end_cmd				  { mon_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
            | CMD_SPRITE_DISPLAY address opt_address end_cmd 	  { mon_display_data($2, $3, 24, 21); }
            | CMD_SPRITE_DISPLAY end_cmd			  { mon_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
            | CMD_TEXT_DISPLAY address opt_address end_cmd 	  { mon_display_memory(0, $2, $3); }
            | CMD_TEXT_DISPLAY end_cmd 				  { mon_display_memory(0, BAD_ADDR, BAD_ADDR); }
            ;

checkpoint_rules: CMD_BREAK address opt_address end_cmd { mon_add_checkpoint($2, $3, FALSE, FALSE, FALSE, FALSE); }
                | CMD_UNTIL address opt_address end_cmd { mon_add_checkpoint($2, $3, FALSE, FALSE, FALSE, TRUE); }
                | CMD_BREAK address opt_address IF cond_expr end_cmd {
                          temp = mon_add_checkpoint($2, $3, FALSE, FALSE, FALSE, FALSE);
                          mon_set_checkpoint_condition(temp, $5); }
                | CMD_WATCH opt_mem_op address opt_address end_cmd { mon_add_checkpoint($3, $4, FALSE,
                              ($2 == e_load || $2 == e_load_store), ($2 == e_store || $2 == e_load_store), FALSE); }
                | CMD_TRACE address opt_address end_cmd { mon_add_checkpoint($2, $3, TRUE, FALSE, FALSE, FALSE); }
                | CMD_BREAK end_cmd { mon_print_checkpoints(); }
                | CMD_UNTIL end_cmd { mon_print_checkpoints(); }
                | CMD_TRACE end_cmd { mon_print_checkpoints(); }
                | CMD_WATCH end_cmd { mon_print_checkpoints(); }
                ;


checkpoint_control_rules: CMD_CHECKPT_ON breakpt_num end_cmd 	 { mon_switch_checkpoint(e_ON, $2); }
                        | CMD_CHECKPT_OFF breakpt_num end_cmd 	 { mon_switch_checkpoint(e_OFF, $2); }
                        | CMD_IGNORE breakpt_num opt_count end_cmd 	 { mon_set_ignore_count($2, $3); }
                        | CMD_DELETE breakpt_num end_cmd 		 { mon_delete_checkpoint($2); }
                        | CMD_DELETE end_cmd				 { mon_delete_checkpoint(-1); }
                        | CMD_CONDITION breakpt_num IF cond_expr end_cmd { mon_set_checkpoint_condition($2, $4); }
                        | CMD_COMMAND breakpt_num STRING end_cmd 	 { mon_set_checkpoint_command($2, $3); }
                        | CMD_COMMAND breakpt_num error end_cmd 	 { return ERR_EXPECT_STRING; }
                        ;

monitor_state_rules: CMD_SIDEFX TOGGLE end_cmd 	       { sidefx = (($2==e_TOGGLE)?(sidefx^1):$2); }
                   | CMD_SIDEFX end_cmd 	       { console_out(console_log, "I/O side effects are %s\n", sidefx ? "enabled" : "disabled"); }
                   | CMD_RADIX RADIX_TYPE end_cmd      { default_radix = $2; }
                   | CMD_RADIX end_cmd
                     {
                         const char *p;

                         if (default_radix == e_hexadecimal)
                             p = "Hexadecimal";
                         else if (default_radix == e_decimal)
                             p = "Decimal";
                         else if (default_radix == e_octal)
                             p = "Octal";
                         else if (default_radix == e_binary)
                             p = "Binary";
                         else
                             p = "Unknown";

                         console_out(console_log, "Default radix is %s\n", p);
                     }

                   | CMD_DEVICE memspace end_cmd       { console_out(console_log, "Setting default device to `%s'\n",
                                                         _mon_space_strings[(int) $2]); default_memspace = $2; }
                   | CMD_QUIT end_cmd 		       { exit_mon = 2; mon_console_close_on_leaving = 1; YYACCEPT; }
                   | CMD_EXIT end_cmd 		       { exit_mon = 1; mon_console_close_on_leaving = 1; YYACCEPT; }
                   ;

monitor_misc_rules: CMD_DISK rest_of_line end_cmd 	{ mon_execute_disk_command($2); }
                  | CMD_PRINT expression end_cmd 	{ console_out(console_log, "\t%d\n",$2); }
                  | CMD_HELP end_cmd 			{ mon_print_help(NULL); }
                  | CMD_HELP rest_of_line end_cmd 	{ mon_print_help($2); }
                  | CMD_SYSTEM rest_of_line end_cmd 	{ printf("SYSTEM COMMAND: %s\n",$2); }
                  | CONVERT_OP expression end_cmd 	{ mon_print_convert($2); }
                  | CMD_CHDIR rest_of_line end_cmd 	{ mon_change_dir($2); }
                  ;

disk_rules: CMD_LOAD filename opt_address end_cmd 			{ mon_load_file($2,$3,FALSE); }
          | CMD_BLOAD filename opt_address end_cmd 			{ mon_load_file($2,$3,TRUE); }
          | CMD_SAVE filename address address end_cmd 		{ mon_save_file($2,$3,$4,FALSE); }
          | CMD_BSAVE filename address address end_cmd 		{ mon_save_file($2,$3,$4,TRUE); }
          | CMD_VERIFY filename address end_cmd 		{ mon_verify_file($2,$3); }
          | CMD_BLOCK_READ expression expression opt_address end_cmd	{ mon_block_cmd(0,$2,$3,$4); }
          | CMD_BLOCK_WRITE expression expression address end_cmd	{ mon_block_cmd(1,$2,$3,$4); }
          ;

cmd_file_rules: CMD_RECORD filename end_cmd 	{ mon_record_commands($2); }
              | CMD_STOP end_cmd 		{ mon_end_recording(); }
              | CMD_PLAYBACK filename end_cmd 	{ playback=TRUE; playback_name = $2; }
              ;

data_entry_rules: CMD_ENTER_DATA address data_list end_cmd { mon_fill_memory($2, BAD_ADDR, $3); }
                | CMD_ENTER_BIN_DATA end_cmd { printf("Not yet.\n"); }
                ;

rest_of_line: R_O_L { $$ = $1; }
            ;

opt_bankname: BANKNAME
	| { $$ = NULL; }
	;

filename: FILENAME
        | error { return ERR_EXPECT_FILENAME; }
        ;

opt_mem_op: MEM_OP { $$ = $1; }
          | { $$ = e_load_store; }
          ;

register: REGISTER          { $$ = new_reg(default_memspace, $1); }
        | memspace REGISTER { $$ = new_reg($1, $2); }
        ;

reg_list: reg_list REG_ASGN_SEP reg_asgn
        | reg_asgn
        ;

reg_asgn: register EQUALS number { mon_set_reg_val(reg_memspace($1), reg_regid($1), $3); }
        ;

opt_count: expression { $$ = $1; }
         | { $$ = -1; }
         ;

breakpt_num: d_number { $$ = $1; }
           | error { return ERR_EXPECT_BRKNUM; }
           ;

opt_address: address { $$ = $1; }
           |         { $$ = BAD_ADDR; }
           ;

address: memloc { $$ = new_addr(e_default_space,$1); if (opt_asm) new_cmd = asm_mode = 1; }
       | memspace memloc { $$ = new_addr($1,$2); if (opt_asm) new_cmd = asm_mode = 1; }
       | LABEL { temp = mon_symbol_table_lookup_addr(e_default_space, $1); 
                 if (temp >= 0)
                    $$ = new_addr(e_default_space, temp);
                 else
                    return ERR_UNDEFINED_LABEL;
               }
       ;

opt_memspace: memspace { $$ = $1; }
            |          { $$ = e_default_space; }
            ;

memspace: MEM_COMP { $$ = e_comp_space; }
        | MEM_DISK8 { $$ = e_disk8_space; }
        | MEM_DISK9 { $$ = e_disk9_space; }
        ;

memloc: memaddr { $$ = $1; if (!CHECK_ADDR($1)) return ERR_ADDR_TOO_BIG; }
      ;

memaddr: number { $$ = $1; }

expression: expression '+' expression { $$ = $1 + $3; }
          | expression '-' expression { $$ = $1 - $3; }
          | expression '*' expression { $$ = $1 * $3; }
          | expression '/' expression { $$ = ($3) ? ($1 / $3) : 1; }
          | '(' expression ')' 	      { $$ = $2; }
          | '(' expression error      { return ERR_MISSING_CLOSE_PAREN; }
          | value  		      { $$ = $1; }
          ;

cond_expr: cond_expr COMPARE_OP cond_expr { $$ = new_cond; $$->is_parenthized = FALSE;
                                            $$->child1 = $1; $$->child2 = $3; $$->operation = $2; }
         | cond_expr COMPARE_OP error 	  { return ERR_INCOMPLETE_COMPARE_OP; }
         | L_PAREN cond_expr R_PAREN 	  { $$ = $2; $$->is_parenthized = TRUE; }
         | L_PAREN cond_expr error 	  { return ERR_MISSING_CLOSE_PAREN; }
         | compare_operand 		  { $$ = $1; }
         ;

compare_operand: register { $$ = new_cond; $$->operation = e_INV; $$->is_parenthized = FALSE;
                            $$->reg_num = $1; $$->is_reg = TRUE; }
               | number   { $$ = new_cond; $$->operation = e_INV; $$->is_parenthized = FALSE;
                            $$->value = $1; $$->is_reg = FALSE; }
               ;

data_list: data_list data_element
         | data_element
         ;

data_element: number { mon_add_number_to_buffer($1); }
            | STRING { mon_add_string_to_buffer($1); }
            ;

value: number { $$ = $1; }
     | register { $$ = mon_get_reg_val(reg_memspace($1), reg_regid($1)); }
     ;

d_number: D_NUMBER { $$ = $1; }
        | B_NUMBER_GUESS { $$ = strtol($1, NULL, 10); }
        | O_NUMBER_GUESS { $$ = strtol($1, NULL, 10); }
        | D_NUMBER_GUESS { $$ = strtol($1, NULL, 10); }
        ;

guess_default: B_NUMBER_GUESS { $$ = resolve_datatype(B_NUMBER,$1); }
             | O_NUMBER_GUESS { $$ = resolve_datatype(O_NUMBER,$1); }
             | D_NUMBER_GUESS { $$ = resolve_datatype(D_NUMBER,$1); }
             ;

number: H_NUMBER { $$ = $1; }
      | D_NUMBER { $$ = $1; }
      | O_NUMBER { $$ = $1; }
      | B_NUMBER { $$ = $1; }
      | guess_default { $$ = $1; }
      ;

assembly_instr_list: assembly_instr_list INST_SEP assembly_instruction
                   | assembly_instruction INST_SEP assembly_instruction
                   | assembly_instruction INST_SEP
                   ;

assembly_instruction: OPCODE asm_operand_mode { $$ = 0;
                                                if ($1) {
                                                    mon_assemble_instr($1, $2);
                                                } else {
                                                    new_cmd = 1;
                                                    asm_mode = 0;
                                                }
                                                opt_asm = 0;
                                              }

post_assemble: assembly_instruction
             | assembly_instr_list { asm_mode = 0; }
             ;

asm_operand_mode: ARG_IMMEDIATE number { if ($2 > 0xff) return ERR_IMM_TOO_BIG;
                                         $$ = join_ints(ASM_ADDR_MODE_IMMEDIATE,$2); }
                | number { if ($1 < 0x100)
                              $$ = join_ints(ASM_ADDR_MODE_ZERO_PAGE,$1);
                           else
                              $$ = join_ints(ASM_ADDR_MODE_ABSOLUTE,$1);
                         }
                | number COMMA REG_X  { if ($1 < 0x100)
                                           $$ = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,$1);
                                        else
                                           $$ = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,$1);
                                      }
                | number COMMA REG_Y  { if ($1 < 0x100)
                                           $$ = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,$1);
                                        else
                                           $$ = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,$1);
                                      }
                | L_PAREN number R_PAREN  { $$ = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,$2); }
                | L_PAREN number COMMA REG_X R_PAREN { $$ = join_ints(ASM_ADDR_MODE_INDIRECT_X,$2); }
                | L_PAREN number R_PAREN COMMA REG_Y { $$ = join_ints(ASM_ADDR_MODE_INDIRECT_Y,$2); }
                | { $$ = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
                | REG_A { $$ = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
                ;


%%

void parse_and_execute_line(char *input)
{
   char *temp_buf;
   int i, rc;

   temp_buf = (char *)xmalloc(strlen(input)+3);
   strcpy(temp_buf,input);
   i = strlen(input);
   temp_buf[i++] = '\n';
   temp_buf[i++] = '\0';
   temp_buf[i++] = '\0';

   make_buffer(temp_buf);
   if ( (rc =yyparse()) != 0) {
       console_out(console_log, "ERROR -- ");
       switch(rc) {
           case ERR_BAD_CMD:
               console_out(console_log, "Bad command:\n");
               break;
           case ERR_RANGE_BAD_START:
               console_out(console_log, "Bad first address in range:\n");
               break;
           case ERR_RANGE_BAD_END:
               console_out(console_log, "Bad second address in range:\n");
               break;
           case ERR_EXPECT_BRKNUM:
               console_out(console_log, "Checkpoint number expected:\n");
               break;
           case ERR_EXPECT_END_CMD:
               console_out(console_log, "Unexpected token:\n");
               break;
           case ERR_MISSING_CLOSE_PAREN:
               console_out(console_log, "')' expected:\n");
               break;
           case ERR_INCOMPLETE_COMPARE_OP:
               console_out(console_log, "Compare operation missing an operand:\n");
               break;
           case ERR_EXPECT_FILENAME:
               console_out(console_log, "Expecting a filename:\n");
               break;
           case ERR_ADDR_TOO_BIG:
               console_out(console_log, "Address too large:\n");
               break;
           case ERR_IMM_TOO_BIG:
               console_out(console_log, "Immediate argument too large:\n");
               break;
           case ERR_EXPECT_STRING:
               console_out(console_log, "Expecting a string.\n");
               break;
           case ERR_UNDEFINED_LABEL:
               console_out(console_log, "Found an undefined label.\n");
               break;
           case ERR_ILLEGAL_INPUT:
           default:
               console_out(console_log, "Wrong syntax:\n");
       }
       console_out(console_log, "  %s\n", input);
       for (i = 0; i < last_len; i++)
           console_out(console_log, " ");
       console_out(console_log, "  ^\n");
       asm_mode = 0;
       new_cmd = 1;
   }
   free_buffer();
}

static int yyerror(char *s)
{
   YYABORT;
   fprintf(stderr, "ERR:%s\n",s);
   return 0;
}

static int resolve_datatype(unsigned guess_type, char *num)
{
   /* FIXME: Handle cases when default type is non-numerical */
   if (default_radix == e_hexadecimal) {
      return strtol(num, NULL, 16);
   }

   if ((guess_type == D_NUMBER) || (default_radix == e_decimal)) {
      return strtol(num, NULL, 10);
   }

   if ((guess_type == O_NUMBER) || (default_radix == e_octal)) {
      return strtol(num, NULL, 8);
   }

   return strtol(num, NULL, 2);
}

