FROM ubuntu:22.04

RUN apt-get update

RUN apt-get install -y ca-certificates openssl git curl wget make autoconf libtool

WORKDIR /root
RUN git clone https://github.com/postmodern/ruby-install
WORKDIR /root/ruby-install
RUN make install

RUN ruby-install ruby

WORKDIR /root
RUN wget https://github.com/postmodern/chruby/releases/download/v0.3.9/chruby-0.3.9.tar.gz
RUN tar -xzvf chruby-0.3.9.tar.gz
WORKDIR /root/chruby-0.3.9
RUN ./scripts/setup.sh
RUN chmod +x /usr/local/share/chruby/chruby.sh

# After running we need to run the following
# `source /usr/local/share/chruby/chruby.sh`
# `git clone https://github.com/Shopify/ess`
# `cd ess`
# `git submodule init`
# `git submodule update`
# `bundle install`
# `bin/rake`
# Compilation should start and takes times