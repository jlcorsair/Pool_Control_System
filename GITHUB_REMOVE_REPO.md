# Remove a GitHub Repository Checklist

Use the commands below to remove the GitHub remote connection and delete the repository at:

https://github.com/jlcorsair/Long_Range_WiFi_Extender

## 1) Go to the local project folder

```powershell
cd "D:\PROJECTS\Long_Range_WiFi_Extender"
```

## 2) Check the current Git remote

```powershell
git remote -v
```

## 3) Remove the GitHub remote from the local repository

```powershell
git remote remove origin
```

## 4) Verify the remote was removed

```powershell
git remote -v
```

## 5) Delete the repository from GitHub

If GitHub CLI is installed:

```powershell
gh auth login
```

Then delete the repository:

```powershell
gh repo delete jlcorsair/Long_Range_WiFi_Extender --yes
```

## 6) Alternative: delete from the GitHub website

Open:

https://github.com/jlcorsair/Long_Range_WiFi_Extender

Then:
- Open Settings
- Scroll down to Danger Zone
- Click Delete this repository
- Confirm the repository name
- Enter your password or use a PAT if prompted

## 7) Optional: remove local Git metadata

If you no longer want this folder to be a Git repository:

```powershell
Remove-Item -Recurse -Force .git
```

## 8) Optional: confirm the local folder is no longer a Git repo

```powershell
git rev-parse --is-inside-work-tree
```

## Quick copy/paste sequence

```powershell
cd "D:\PROJECTS\Long_Range_WiFi_Extender"
git remote -v
git remote remove origin
git remote -v
```

```powershell
gh auth login
gh repo delete jlcorsair/Long_Range_WiFi_Extender --yes
```
