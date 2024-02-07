#!/bin/bash

# Define the environment variable for the configuration file path
export VSYNC_CONFIG_PATH=~/.vsync_rc

# Remove existing installation if present
if [ -f "/usr/local/bin/vaultsync" ]; then
    sudo rm /usr/local/bin/vaultsync
fi

# Move back to the project root directory
cd ..

# Check if build directory exists, if not, create it
if [ ! -d "build" ]; then
    mkdir -p build
fi

# Build the C project CLI
cd build
cmake ..
make

# Copy the executable to a location in PATH
sudo cp vaultsync /usr/local/bin

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
