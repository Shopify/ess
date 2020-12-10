apt-get update
apt-get install -y build-essential zlib1g-dev libssl-dev libreadline6-dev libyaml-dev wget libffi-dev gcc g++ make bison libtool-bin autoconf
cd `mktemp -d`
curl -L "https://cache.ruby-lang.org/pub/ruby/2.7/ruby-2.7.1.tar.gz" | tar -xzf-
cd ruby-2.7.1
./configure --prefix=/usr/local
make
make install
gem install bundler
