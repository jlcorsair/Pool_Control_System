# GitHub Commit and Push Checklist for Pool Control System

Use the commands below from the project folder to verify whether the repository is up to date, identify what needs to be committed, and push it to GitHub using the username `jlcorsair`.

> Current repo note: this workspace currently shows local deletions and untracked files, and there is no remote configured yet.

## 1) Go to the project folder

```powershell
cd "D:\PROJECTS\HOA\Pool_Control_System"
```

- Check: command should complete without error.
- Expected result: you are in the project repository folder.

## 2) Check repository status

```powershell
git status --short --branch
```

- Check: output shows branch name plus changed/untracked files.
- Expected result: confirm working tree state and branch.

## 3) Check whether this folder is a Git repository

```powershell
git rev-parse --is-inside-work-tree
```

- Check: output is `true`.
- Expected result: verifies the current directory is inside a Git repo.

## 4) Check current branch and tracking info

```powershell
git branch -vv
```

- Check: you see the current branch marked with `*` and remote tracking info if configured.
- Expected result: know which branch is active and whether it tracks a remote.

## 5) Check remote configuration

```powershell
git remote -v
```

- Check: output lists `origin` URLs or shows no remotes.
- Expected result: verify whether Git has a remote and its URL.

## 6) Verify the GitHub remote exists

```powershell
git ls-remote https://github.com/jlcorsair/Pool_Control_System.git
```

- Check: returns refs if repo exists, or error if missing.
- Expected result: confirm GitHub repo is accessible.

## 7) Fetch the latest remote state

```powershell
git fetch origin
```

- Check: command completes without fatal errors.
- Expected result: update local refs from remote.

## 8) Compare local branch to remote branch

```powershell
git log --oneline --left-right HEAD...origin/main
```

- Check: output shows commits only on local or remote side.
- Expected result: know if local and remote diverge.

## 9) Pull latest changes if a remote exists

```powershell
git pull origin main
```

- Check: command completes successfully or reports merge/rebase needed.
- Expected result: bring your local branch up to date with `origin/main`.

If you prefer rebase instead of merge:

```powershell
git pull --rebase origin main
```

- Check: command runs and rebases local commits onto updated remote.
- Expected result: keep history linear while updating from remote.

## 10) Review the changes to be committed

```powershell
git diff --stat
git diff
```

- Check: `--stat` shows file counts and sizes; `git diff` shows actual content changes.
- Expected result: inspect what will be committed before staging.

## 11) Stage files for commit

Stage everything:

```powershell
git add .
```

- Check: `git status --short` should show staged files with `A`, `M`, or `D`.
- Expected result: prepare changes for commit.

Or stage only specific files:

```powershell
git add README.md
```

- Check: only the specified file appears staged.
- Expected result: stage only the files you want.

## 12) Commit the changes

```powershell
git commit -m "Update Pool Control System files"
```

- Check: output confirms commit was created.
- Expected result: record staged changes in local history.

## 13) Push to GitHub

If the repository has no remote yet, add one first:

```powershell
git remote add origin https://github.com/jlcorsair/Pool_Control_System.git
```

Or use SSH:

```powershell
git remote add origin git@github.com:jlcorsair/Pool_Control_System.git
```

- Check: `git remote -v` shows the new origin URL.
- Expected result: remote is configured for push/pull.

Then push:

```powershell
git push -u origin main
```

- Check: output indicates refs were pushed and upstream is set.
- Expected result: local `main` is uploaded to GitHub.

## 14) If the branch already exists on GitHub, use

```powershell
git push
```

- Check: output confirms changes are pushed.
- Expected result: update the tracked branch without resetting upstream.

## 15) Quick full check sequence

Run this sequence when you want to verify everything at once:

```powershell
cd "D:\PROJECTS\HOA\Pool_Control_System"
git status --short --branch
git remote -v
git fetch origin
git status
git pull origin main
git add .
git commit -m "Update Pool Control System files"
git push -u origin main
```

- Check: each command should succeed in order.
- Expected result: status check, remote validation, update, commit, and push completed.

## Notes

- If Git prompts for credentials, use a GitHub Personal Access Token instead of your password.
- If there are merge conflicts after pulling, resolve them manually, then run:

```powershell
git add .
git commit
```
- After successful push, verify the repository appears in your browser under your GitHub account.
