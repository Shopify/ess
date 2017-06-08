apt-get update
apt-get install -y build-essential zlib1g-dev libssl-dev libreadline6-dev libyaml-dev wget libffi-dev gcc g++ make bison libtool autoconf git
cd `mktemp -d`
curl -L "https://cache.ruby-lang.org/pub/ruby/2.3/ruby-2.3.3.tar.gz" | tar -xzf-
cd ruby-2.3.3
./configure --prefix=/usr/local
make
make install
gem install bundler
