FROM gitpod/workspace-full-vnc

USER root
# add your tools here
RUN apt-get update && apt-get install -y \
  libsdl2-dev
