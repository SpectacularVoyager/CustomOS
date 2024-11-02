tmux new-session -d 'nvim Makefile'
tmux split-window -h 
tmux new-window 'nvim src/kernel.c'
tmux split-window -h 'make log'
tmux new-window 'nvim src/drivers/xhci/xhci.c'
tmux split-window -h 'nvim src/drivers/xhci/xhci.c'
tmux -2 attach-session -d
