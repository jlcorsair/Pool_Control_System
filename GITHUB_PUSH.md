# GitHub Safe Push and Pull Workflow

This file contains a safe, chronological Git workflow for checking whether a repository exists locally or remotely, comparing state, pulling updates, staging changes, committing, and pushing to GitHub.

## 1) Check current repository status

Run this first in the project folder:

```powershell
git status
```

## 2) Check whether the current folder is a Git repository

```powershell
git rev-parse --is-inside-work-tree
```

If it returns `true`, the folder is already a Git repository.

## 3) Check whether a remote repository exists

```powershell
git remote -v
```

If nothing is shown, the repository has no remote configured yet.

## 4) Check whether the repository has local commits not pushed

```powershell
git log --oneline --decorate --graph --all -10
```

## 5) Check whether the local branch exists

```powershell
git branch
```

## 6) Check whether the remote branch exists

```powershell
git branch -r
```

## 7) Compare local and remote branches

```powershell
git fetch origin
git status
```

To compare your branch with the remote branch:

```powershell
git log --oneline --left-right HEAD...origin/main
```

## 8) If remote exists, pull latest changes safely

If you are on the main branch:

```powershell
git pull origin main
```

If you want to update from the current branch:

```powershell
git pull origin
```

If conflicts appear, resolve them manually and continue with:

```powershell
git add .
git commit
```

## 9) If no remote exists, add one

```powershell
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
```

Or with SSH:

```powershell
git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPOSITORY.git
```

## 10) Create or switch to a branch (recommended)

```powershell
git checkout -b feature/my-change
```

Or switch to an existing branch:

```powershell
git checkout main
```

## 11) Stage all changes

```powershell
git add .
```

Or stage specific files:

```powershell
git add README.md
git add src\app.py
```

## 12) Review staged changes

```powershell
git diff --cached
```

## 13) Commit locally

```powershell
git commit -m "Describe your changes"
```

## 14) Check commit history

```powershell
git log --oneline -5
```

## 15) Push to GitHub

If the branch is new and has no upstream yet:

```powershell
git push -u origin feature/my-change
```

If the branch already exists on remote:

```powershell
git push
```

## 16) Create a Pull Request (optional)

If using GitHub CLI:

```powershell
gh pr create --base main --head feature/my-change --fill
```

## Recommended safe workflow

Use this order for everyday work:

```powershell
git status
git remote -v
git fetch origin
git status
git pull origin main
git add .
git commit -m "Describe your changes"
git push -u origin main
```

If you are using a feature branch, replace the last two commands with:

```powershell
git push -u origin feature/my-change
```

## Useful emergency checks

```powershell
git status
git branch
git branch -r
git log --oneline --decorate --graph --all
```
