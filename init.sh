#!/usr/bin/env bash
set -e

SCRIPT_PATH="$(realpath "$0")"

# Ask for project name
read -rp "Project name: " PROJECT_NAME

if [[ -z "$PROJECT_NAME" ]]; then
  echo "Error: project name cannot be empty."
  exit 1
fi

# Replace cpp_template with the new project name in all relevant files
sed -i "s/cpp_template/$PROJECT_NAME/g" CMakeLists.txt Makefile src/main.cpp

# Remove origin
git remote remove origin 2>/dev/null || true

# Rename current branch to main
git branch -m main

# Delete local dist branch if it exists
git branch -d dist 2>/dev/null || git branch -D dist 2>/dev/null || true

# Stage the renamed files and self-deletion in one commit
git add CMakeLists.txt Makefile src/main.cpp
git rm --cached "$SCRIPT_PATH" 2>/dev/null || true
rm -f "$SCRIPT_PATH"
git add -A

git commit -m "init: $PROJECT_NAME"
