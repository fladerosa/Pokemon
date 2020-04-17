#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "common_utils.h"

const static struct{
    op_code operation;
    char* name; 
}message_string [] = {
    {NEW_POKEMON, "NEW_POKEMON"},
    {APPEARED_POKEMON, "APPEARED_POKEMON"},
    {CATCH_POKEMON, "CATCH_POKEMON"},
    {CAUGHT_POKEMON, "CAUGHT_POKEMON"},
    {GET_POKEMON, "GET_POKEMON"},
    {SUSCRIPTOR, "SUSCRIPTOR"},
    {ERROR, "ERROR"}
};

op_code stringToEnum(char* message);

#endif