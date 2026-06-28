# GitHub Setup Commands for a Fresh Windows Machine

Use these commands in PowerShell to set up Git and GitHub on a clean Windows installation.

> Note: On GitHub, use a Personal Access Token (PAT) instead of your account password for HTTPS authentication.

## 1) Install Git

```powershell
winget install --id Git.Git -e
```

## 2) Verify installation

```powershell
git --version
```

## 3) Set your Git identity

Replace the values with your own name and email.

```powershell
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

## 4) Set useful defaults

```powershell
git config --global init.defaultBranch main
git config --global core.autocrlf true
git config --global pull.rebase false
git config --global credential.helper manager-core
```

## 5) Set your GitHub username for HTTPS

```powershell
git config --global credential.username "YOUR_GITHUB_USERNAME"
```

## 6) Optional: Configure GitHub CLI (recommended)

```powershell
winget install --id GitHub.cli -e
```

Verify:

```powershell
gh --version
```

## 7) Create an SSH key (optional, recommended for secure access)

```powershell
ssh-keygen -t ed25519 -C "your.email@example.com"
```

Press Enter to accept the default file location and set a passphrase if desired.

## 8) Start the SSH agent and add the key

```powershell
Get-Service ssh-agent | Set-Service -StartupType Automatic
Start-Service ssh-agent
ssh-add $env:USERPROFILE\.ssh\id_ed25519
```

## 9) Copy your public SSH key

```powershell
Get-Content $env:USERPROFILE\.ssh\id_ed25519.pub
```

Then add that key to GitHub:
- Open https://github.com/settings/keys
- Click New SSH key
- Paste the key and save it

## 10) Authenticate with GitHub using GitHub CLI

```powershell
gh auth login
```

Choose:
- GitHub.com
- HTTPS
- Login with a web browser
- Follow the browser instructions

## 11) Alternative: Use a Personal Access Token (PAT) for HTTPS

Create a PAT in GitHub:
- Go to https://github.com/settings/tokens
- Generate a new token with repo access

Then use it when Git prompts for a password:

```powershell
git clone https://github.com/OWNER/REPO.git
```

When prompted for a password, enter your GitHub PAT instead of your account password.

## 12) Check your Git configuration

```powershell
git config --global --list
```

## 13) Test GitHub access

```powershell
git ls-remote https://github.com/OWNER/REPO.git
```

Or with SSH:

```powershell
git ls-remote git@github.com:OWNER/REPO.git
```

## 14) Create a new repository locally

```powershell
mkdir my-project
cd my-project
git init
git branch -M main
```

## 15) Add remote repository

```powershell
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
```

Or with SSH:

```powershell
git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPOSITORY.git
```

## 16) Commit and push

```powershell
git add .
git commit -m "Initial commit"
git push -u origin main
```
