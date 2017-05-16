# -*- coding: utf-8 -*-
MRuby::Gem::Specification.new('mruby-mpdecimal') do |spec|
  spec.authors = ["Simon Génier"]
  spec.license = "BSD"
  spec.summary = "Decimal through mpdecimal"

  spec.cc do
    cc.flags += %w(-Wno-declaration-after-statement)
    cc.defines += %w(CONFIG_64 ASM HAVE_UINT128_T)
  end
end
