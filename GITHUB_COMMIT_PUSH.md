# GitHub Commit and Push Checklist for Pool Control System

Use the commands below from the project folder to verify whether the repository is up to date, identify what needs to be committed, and push it to GitHub using the username `jlcorsair`.

> Current repo note: this workspace may need the GitHub repo created before pushing, and GitHub rejects any file larger than 100 MB. If the remote is missing, use `gh repo create` with `--source=. --remote=origin --push`.

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

## 6) Confirm the GitHub repository exists

```powershell
gh repo view jlcorsair/Pool_Control_System
```

- Check: command succeeds if the repo exists.
- Expected result: remote repository exists and is accessible.

If the repository does not exist yet, create it from the current local repo:

```powershell
gh repo create Pool_Control_System --public --source=. --remote=origin --push
```

- Check: command creates the repo, adds `origin`, and pushes local commits.
- Expected result: GitHub repo is created and `origin` is configured.

If the remote exists but the URL is wrong, update it:

```powershell
git remote set-url origin https://github.com/jlcorsair/Pool_Control_System.git
```

- Check: `git remote -v` shows the corrected URL.
- Expected result: `origin` points to the correct GitHub repository.

## 7) Verify the remote refs

```powershell
git ls-remote origin
```

- Check: returns refs from the remote.
- Expected result: confirm GitHub repo is reachable.

## 8) Fetch the latest remote state

```powershell
git fetch origin
```

- Check: command completes without fatal errors.
- Expected result: update local refs from remote.

## 9) Compare local branch to remote branch

```powershell
git log --oneline --left-right HEAD...origin/main
```

- Check: output shows commits only on local or remote side.
- Expected result: know if local and remote diverge.

If the remote branch does not exist yet, skip this step and proceed to commit and push.

## 10) Pull latest changes if a remote exists

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

## 11) Review the changes to be committed

```powershell
git diff --stat
git diff
```

- Check: `--stat` shows file counts and sizes; `git diff` shows actual content changes.
- Expected result: inspect what will be committed before staging.

## 12) Check for oversized files

```powershell
git ls-tree -r --long HEAD | sort -k4 -rn | head -n 20
```

- Check: no file is larger than 100 MB.
- Expected result: ensure GitHub push will not be blocked by large files.

If you find large files, remove them from the repo history or exclude them with `.gitignore` before pushing.

## 13) Stage files for commit

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

## 14) Commit the changes

```powershell
git commit -m "Update Pool Control System files"
```

- Check: output confirms commit was created.
- Expected result: record staged changes in local history.

## 15) Push to GitHub

```powershell
git push -u origin main
```

- Check: output indicates refs were pushed and upstream is set.
- Expected result: local `main` is uploaded to GitHub.

## 16) If the branch already exists on GitHub, use

```powershell
git push
```

- Check: output confirms changes are pushed.
- Expected result: update the tracked branch without resetting upstream.

## 17) Quick full check sequence

Run this sequence when you want to verify everything at once:

```powershell
cd "D:\PROJECTS\HOA\Pool_Control_System"
git status --short --branch
git remote -v
git ls-remote origin
git fetch origin
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
- If the remote repository is missing, use `gh repo create Pool_Control_System --public --source=. --remote=origin --push`.
- After successful push, verify the repository appears in your browser under your GitHub account.
