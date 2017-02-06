#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);
   
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

  return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */

  char line[FILENAME_SIZE];
  char* curIdxName = ".beargit/.index";
  char* newIdxName = ".beargit/.newindex";
  FILE* fin = fopen(curIdxName, "r");
  FILE* fout = fopen(newIdxName, "w");

  while(fgets(line, sizeof(line), fin))
  {
    strtok(line, "\n");
    if(strcmp(line, filename) == 0)
    {
      while(fgets(line, sizeof(line), fin))
      {
        fprintf(fout, "%s", line);
      }

      fclose(fin);
      fclose(fout);
      fs_mv(newIdxName, curIdxName);
      return 0;
    }

    fprintf(fout, "%s\n", line);
  }

  fclose(fin);
  fclose(fout);
  fprintf(stderr, "ERROR: File %s not tracked\n", filename);
  return 1;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */

  //TODO KMP

  char* msgP = &(*msg);
  while(*msgP != 0)
  {
    char* checkP = &(*go_bears);
    char* tmpP = &(*msgP);
    while(*tmpP != 0 && *checkP != 0)
    {
      if(*tmpP != *checkP) break;
      tmpP++;
      checkP++;
    }

    if(*checkP == 0) return 1;
    msgP++;
  }

  return 0;
}

void next_commit_id_hw1(char* commit_id) {
  /* COMPLETE THE REST */
  char* firstSmall = &(*commit_id);

  while(*firstSmall != 0 && (*firstSmall == 'c' || *firstSmall == '0')) firstSmall++;

  if(*firstSmall == '1') *firstSmall = '6';
  else if(*firstSmall == '6') *firstSmall = 'c';

  char* cleaner = &(*commit_id);
  while(cleaner != firstSmall)
  {
    *cleaner = '1';
    cleaner++;
  }

  commit_id -= COMMIT_ID_BRANCH_BYTES;
}

int beargit_commit_hw1(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  //!!!!!!!!!!!!!!!!!!!!!!TODO Other strcat change
  char newCommitPath[20 + COMMIT_ID_SIZE] = ".beargit/";
  strcat(newCommitPath, commit_id);
  /* COMPLETE THE REST */
  fs_mkdir(newCommitPath);
  char oldPrevPath[sizeof(newCommitPath)];
  sprintf(oldPrevPath, "%s/.prev", newCommitPath);
  fs_cp(".beargit/.prev", oldPrevPath);

  char oldIndexPath[sizeof(newCommitPath)];
  sprintf(oldIndexPath, "%s/.index", newCommitPath);
  fs_cp(".beargit/.index", oldIndexPath);

  FILE* fin = fopen(".beargit/.index", "r");
  char fileName[FILENAME_SIZE];
  while(fgets(fileName, FILENAME_SIZE, fin))
  {
    strtok(fileName, "\n");
    char oldFilePath[sizeof(newCommitPath)+sizeof(fileName)];
    sprintf(oldFilePath, "%s/%s", newCommitPath, fileName);
    fs_cp(fileName, oldFilePath);
  }

  fclose(fin);
  write_string_to_file(".beargit/.newPrev", commit_id);
  write_string_to_file(strcat(newCommitPath, "/.msg"), msg);
  fs_mv(".beargit/.newPrev", ".beargit/.prev");

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  char fileName[FILENAME_SIZE];
  int count = 0;
  printf("Tracked files:\n");
  FILE* fin = fopen(".beargit/.index", "r");
  do
  {
    if(fgets(fileName, FILENAME_SIZE, fin) == NULL)
    {
      break;
    }
    else
    {
      printf("%s", fileName);
      count++;
    }
  }while(1);

  fclose(fin);
  printf("%d files total\n", count);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* COMPLETE THE REST */
  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  if(*commit_id == '0') 
  {
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }

  fprintf(stdout, "\n");
  do
  {
    fprintf(stdout, "commit %s\n", commit_id);
    char prevPath[20+COMMIT_ID_SIZE];
    sprintf(prevPath, ".beargit/%s", commit_id);
    char copyPrevPath[sizeof(prevPath)];
    strcpy(copyPrevPath, prevPath);
    read_string_from_file(strcat(prevPath, "/.prev"), commit_id, COMMIT_ID_SIZE);

    char msg[MSG_SIZE];
    read_string_from_file(strcat(copyPrevPath, "/.msg"), msg, MSG_SIZE);
    fprintf(stdout, "%s\n", msg);

  }while(*commit_id != '0');

  return 0;
}

// ---------------------------------------
// HOMEWORK 2 
// ---------------------------------------

// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.
int beargit_commit(const char* msg) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  if (strlen(current_branch) == 0) {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
    return 1;
  }

  return beargit_commit_hw1(msg);
}

const char* digits = "61c";

void next_commit_id(char* commit_id) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // Fill the first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    commit_id[i] = digits[n%3];
    n /= 3;
  }

  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_hw1(commit_id + COMMIT_ID_BRANCH_BYTES);
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[BRANCHNAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
      break;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  FILE* fin = fopen(".beargit/.branches", "r");
  char line[BRANCHNAME_SIZE];
  while(fgets(line, sizeof(line), fin))
  {
    strtok(line, "\n");
    if(strlen(current_branch) == 0) 
    {
      fprintf(stdout, "%s\n", line);
    }
    else
    {
      if(strcmp(current_branch, line) == 0)
      {
        fprintf(stdout, "* %s\n", line);
      }
    }
  }

  fclose(fin);
  return 0;
}

/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
  //Remove current indexed files
  FILE* fin = fopen(".beargit/.index", "r");
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fin))
  {
    strtok(line, "\n");
    fs_rm(line);
  }

  fclose(fin);

  //Copy Index checked out

  fs_rm(".beargit/.index");
  if(*commit_id != '0')
  {
    char checkoutDir[COMMIT_ID_SIZE + 10];
    char checkoutIndex[COMMIT_ID_SIZE+20];
    sprintf(checkoutDir, ".beargit/%s", commit_id);
    sprintf(checkoutIndex, "%s/.index", checkoutDir);
    fs_cp(checkoutIndex, ".beargit/.index");

      //Copy the files from checked out commit
    fin = fopen(".beargit/.index", "r");
    while(fgets(line, sizeof(line), fin))
    {
      strtok(line, "\n");
      char checkoutFile[strlen(checkoutDir) + sizeof(line)];
      sprintf(checkoutFile, "%s/%s", checkoutDir, line);
      fs_cp(checkoutFile, line);
    }

    fclose(fin);
  }
  else
  {
      FILE* fout = fopen(".beargit/.index", "w");
      fclose(fout);
  }

  //Set .beargit/.prev to the commit.
  write_string_to_file(".beargit/.prev", commit_id);


  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  if(commit_id == 0 || strlen(commit_id) != COMMIT_ID_BYTES) return 0;
  for(int i = 0; i < COMMIT_ID_BYTES; i++)
  {
    int find = 0;
    for(int j = 0; j < strlen(digits); j++)
    {
      if(digits[j] == commit_id[i])
      {
        find = 1;
        break;
      }
    }
    if(!find) return 0;
  }
  
  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", "current_branch", BRANCHNAME_SIZE);

  // If not detached, leave the current branch by storing the current HEAD into that branch's file...
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

  // ...and setting the current branch to none (i.e., detached).
  write_string_to_file(".beargit/.current_branch", "");

  // Check whether the argument is a commit ID. If yes, we just change to detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }
    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  } else if (!branch_exists && new_branch) {
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch to change into.
  char* branch_file = ".beargit/.branch_"; 
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fs_cp(".beargit/.prev", branch_file); 
  }

  write_string_to_file(".beargit/.current_branch", branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}
