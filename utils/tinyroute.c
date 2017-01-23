/* tinyroute.c
 * Copyright (C) 2017 Intel Corporation
 * Inspired by tinymix from tinyalsa package
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <tinyroute/audio_route.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "../src/audio_route_private.h"

/* functionality added on top of audio_route for testing */
void _list_paths(struct audio_route *ar)
{
    unsigned int i;
  
    if (ar == NULL || ar->num_mixer_paths ==0)
	return;
  
    for (i=0; i<ar->num_mixer_paths; i++) {
	printf("index %d: path %s\n", i, ar->mixer_path[i].name);
    }
}

int _apply_path_index(struct audio_route *ar, unsigned i)
{
    int ret;
    
    if (ar == NULL || ar->num_mixer_paths ==0) {
	ret = -1;
	goto _end;
    }
    
    if (i < ar->num_mixer_paths) {
	ret = audio_route_apply_path(ar, ar->mixer_path[i].name);
    } else {
	ret = -1;
    }
    
_end:
    return ret;
}

/* Argument parsing, etc */

struct cmd {
    const char *filename;
    bool parse_only;
    unsigned int card;
};

void cmd_init(struct cmd *cmd)
{
    cmd->filename = NULL;
    cmd->card = 0;
}

int cmd_parse_arg(struct cmd *cmd, int argc, const char **argv)
{
    if (argc < 1) {
        return 0;
    }

    if (argv[0][0] != '-') {
        cmd->filename = argv[0];
        return 1;
    }

    if ((strcmp(argv[0], "-p") == 0) || (strcmp(argv[0], "--parse") == 0)) {
        cmd->parse_only = true;
	return 1;
    }

    if (argc < 2) {
        fprintf(stderr, "option '%s' is missing argument\n", argv[0]);
        return -1;
    }
 
    if ((strcmp(argv[0], "-D") == 0) || (strcmp(argv[0], "--card") == 0)) {
        if (sscanf(argv[1], "%u", &cmd->card) != 1) {
            fprintf(stderr, "failed parsing card number '%s'\n", argv[1]);
            return -1;
        }
    } 

    return 2;
}

int cmd_parse_args(struct cmd *cmd, int argc, const char **argv)
{
    int i = 0;
    while (i < argc) {
        int j = cmd_parse_arg(cmd, argc - i, &argv[i]);
        if (j < 0){
            break;
        }
        i += j;
    }

    return i;
}

struct ctx {
    struct audio_route *ar;
};

int ctx_init(struct ctx* ctx, const struct cmd *cmd)
{
    if (cmd->filename == NULL) {
        fprintf(stderr, "filename not specified\n");
        return -1;
    }

    ctx->ar = audio_route_init(cmd->card, cmd->filename);
    if (ctx->ar == NULL) {
        fprintf(stderr, "failed to init audio route for card %d, XML '%s'\n",
		cmd->card,
		cmd->filename);
        return -1;
    }

    return 0;
}

void ctx_free(struct ctx *ctx)
{
    if (ctx == NULL) {
        return;
    }
    if (ctx->ar != NULL) {
      audio_route_free(ctx->ar);
    }
}

void print_usage(const char *argv0)
{
    fprintf(stderr, "usage: %s file.xml [options]\n", argv0);
    fprintf(stderr, "options:\n");
    fprintf(stderr, "-D | --card   <card number>    The card to configure\n");
    fprintf(stderr, "-p | --parse                   Parse XML file only\n");
}

int main(int argc, const char **argv)
{
    struct cmd cmd;
    struct ctx ctx;
    char inp[16];
    
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    cmd_init(&cmd);
    if (cmd_parse_args(&cmd, argc - 1, &argv[1]) < 0) {
        return EXIT_FAILURE;
    }

    if (ctx_init(&ctx, &cmd) < 0) {
        return EXIT_FAILURE;
    }

    _list_paths(ctx.ar);

    if (cmd.parse_only) {
	audio_route_reset(ctx.ar);
	exit(0);
    }
    

    while (1) {
	int n;
	
	printf("Next action:\n");
	printf("l: list paths \n");
	printf("s<x>: select path <x> \n");
	printf("r: reset to default values\n");
	printf("q: exit \n");
	
	n = scanf("%s", inp);
	if (n!= 1)
	    exit(1);      
	
	if (strncmp(inp, "l", 1) == 0) {
	    _list_paths(ctx.ar);
	} else if(strncmp(inp, "q", 1) == 0) {
	    goto _end;
	} else if(strncmp(inp, "r", 1) == 0) {
	    audio_route_reset(ctx.ar);
	    audio_route_update_mixer(ctx.ar);
	} else if (strncmp(inp, "s", 1) == 0) {
	    int i;
	    
	    i = atoi(&inp[1]);
	    printf("applying path index %d\n",i);
	    _apply_path_index(ctx.ar, i);
	    audio_route_update_mixer(ctx.ar);
	}
    }
_end:
    ctx_free(&ctx);
    return EXIT_SUCCESS;
}
