#!/bin/bash

# Define the environment variable for the configuration file path
export VSYNC_CONFIG_PATH=~/.vsync_rc

# Prompt for username
read -p "Enter your username: " username

# Prompt for email
read -p "Enter your email: " email

# Create the configuration file with username and email
echo "$username $email" > "$VSYNC_CONFIG_PATH"

echo "Configuration file created at $VSYNC_CONFIG_PATH with the following content:"
echo "$username $email"

# Check if .bashrc exists and add the source line if so
if [ -f ~/.bashrc ]; then
    echo -e "\n# Source VSYNC configuration script" >> ~/.bashrc
    echo "export VSYNC_CONFIG_PATH=~/.vsync_rc" >> ~/.bashrc
fi

# Check if .zshrc exists and add the source line if so
if [ -f ~/.zshrc ]; then
    echo -e "\n# Source VSYNC configuration script" >> ~/.zshrc
    echo "export VSYNC_CONFIG_PATH=~/.vsync_rc" >> ~/.zshrc
fi

echo "Setup complete. Restart your shell or run 'source ~/.bashrc' or 'source ~/.zshrc'."
