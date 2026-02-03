FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# System tools
RUN apt-get update && apt-get install -y \
    vim \
    bat \
    bash \
    curl \
    jq \
    && rm -rf /var/lib/apt/lists/*

# bat symlink (same as your example)
RUN ln -s /usr/bin/batcat /usr/local/bin/bat

# Install Node.js (LTS)
RUN curl -fsSL https://deb.nodesource.com/setup_18.x | bash - \
    && apt-get install -y nodejs

# Create dev user FIRST (same pattern)
RUN useradd -m -s /bin/bash dev
ENV HOME=/home/dev

USER dev
WORKDIR ${HOME}

# Default shell
CMD ["/bin/bash"]

