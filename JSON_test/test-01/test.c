/**
 * @Author: qlc
 * @Date:   2018-10-30T10:46:34+08:00
 * @Email:  qlcx@tom.com
 * @Filename: test.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-30T12:09:30+08:00
 * @License: GPL
 */
#include <cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  cJSON *root = NULL, *val = NULL, *arrnode = NULL, *node = NULL;

  char *str;

  root = cJSON_CreateObject();
  if (root == NULL)
    goto cJSON_CreateObject_err;
  val = cJSON_AddNumberToObject(root, "Type", 0);
  if (val == NULL)
    goto destroy_root;
  val = cJSON_AddStringToObject(root, "DateTime", "2018-10-08 14:25:36");
  if (val == NULL)
    goto destroy_root;
  arrnode = cJSON_AddArrayToObject(root, "Values");
  if (arrnode == NULL)
    goto destroy_root;
  node = cJSON_CreateObject();
  if (node == NULL)
    goto destroy_root;
  cJSON_AddItemToArray(arrnode, node);
  val = cJSON_AddStringToObject(node, "DateTime", "2018-10-08 14:25:36");
  if (val == NULL)
    goto destroy_root;
  val = cJSON_AddNumberToObject(node, "Value", 15);
  if (val == NULL)
    goto destroy_root;

  str = cJSON_Print(root);
  printf("%s\n", str);
  free(str);
  cJSON_Delete(root);
  return 0;
destroy_root:
  cJSON_Delete(root);
cJSON_CreateObject_err:
  return -1;
}
