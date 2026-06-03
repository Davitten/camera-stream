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
docker buildx build --platform linux/arm64 -t carlo:arm64 .
```