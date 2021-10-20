# -*- coding: utf-8 -*-
MRuby::Gem::Specification.new('mruby-mpdecimal') do |spec|
  spec.authors = ["Simon Génier"]
  spec.license = "BSD"
  spec.summary = "Decimal through mpdecimal"

  spec.cc do
    cc.flags += %w(-Wno-declaration-after-statement)
    cc.defines += %w(CONFIG_64 HAVE_UINT128_T)
    cc.defines += RUBY_PLATFORM =~ /x86/i ? %w(ASM) : %w(ANSI)
  end
end
