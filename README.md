Install tmux on the remote system

To install tmux on the remote system, you can use the following command:

```bash
sudo apt-get update && sudo apt-get install tmux
```

Start the tmux session
```bash
tmux
```

Start the build and exit session by pressing `Ctrl + b` followed by `d` to detach from the tmux session. You can reattach to the session later using:

```bash
tmux attach
```

Build the docker image for arm64 architecture using buildx:
```bash
docker buildx build --platform linux/arm64 -t camera-stream:arm64 .
```

# Deploying the application to the remote system

Set the USER environment variable to your username on the remote system:
```bash
export REMOTE_USER=your_username
```

Deploy the binary to the remote system using scp:
```bash
ssh $REMOTE_USER@raspberrypi.local "sudo systemctl kill camera-stream.service" && \
scp cmake-build-release-docker/camera-stream $REMOTE_USER@raspberrypi.local:/home/$REMOTE_USER/ && \
ssh $REMOTE_USER@raspberrypi.local "sudo mv /home/$REMOTE_USER/camera-stream /usr/local/bin/"
```

Deploy the service file to the remote system using scp:
```bash
scp camera-stream.service mada@raspberrypi.local:/home/$REMOTE_USER/ && \
ssh $REMOTE_USER@raspberrypi.local "sudo mv /home/$REMOTE_USER/camera-stream.service /etc/systemd/system/"
```

# Enable and start the service on the remote system
SSH into the remote system and enable the service:
```bash
ssh $REMOTE_USER@raspberrypi.local \
"sudo systemctl daemon-reload && sudo systemctl enable camera-stream.service && sudo systemctl start camera-stream.service"
```

