#ifndef __EXPORT_H__
#define __EXPORT_H__

#define EXPORT_DIR "./export"

/**
 * @brief Creates a directory at the specified path.
 *
 * This function checks whether a directory already exists at the specified path.
 * If the directory does not exist, it creates one with the permission mode 0777.
 *
 * @param path - The path of the directory to create.
 * @return 0 if successful, -1 if an error occurred.
 */
int export__create_dir(const char* path);

/**
 * @brief Compiles Graphviz DOT files into PNG images.
 *
 * This function uses the `dot` command from Graphviz to convert all `.dot` files in the
 * `./export/` directory into `.png` images. The original `.dot` files are then removed.
 */
void export__compile_dot();

/**
 * @brief Removes the directory at the specified path.
 *
 * This function checks whether a directory exists at the specified path.
 * If the directory exists, it removes it using the `rm -r` command.
 *
 * @param directory - The path of the directory to remove.
 */
void export__remove_dir(const char* directory);

#endif // __EXPORT_H__