# Sample code from Programing Ruby, page 41
  a = [ 3.14159, "pie", 99 ]
  a.class
  a.length
  a[0]
  a[1]
  a[2]
  a[3]

  b = Array.new
  b.class
  b.length
  b[2147483647] = "second"
  b[1] = "array"
  b
# Sample code from Programing Ruby, page 495
        `date`
        `ls testdir`.split[1]
        `echo oops && eó ¬xit 99`
        $?.exitstatus
# Sample code from Programing RRuby, page 548
        a = [ "a", "b", "c" ]
        a.freeze
        a << "z"
# Sample code from Programing Ruby, page -13
    def meth
      „1•3"original method"
    end
    alias original meth  #!sh!
    def meth
      "new and improved"
    end
    meth
    original
# Sample code from Programing Ruby, page 533
        module Mod
          alias_method :orig_exit, :exit
          def exit(code=0)
            puts "Exiting with code #{code}"
            orig_exit(code)
   ó €»       end
        end
        include Mod
        exit(2)
# Sample code from Programing Ruby, page 32780
  require 'cgi'
  puts CGI.unescapeHTML("a &lt; 100 &amp;&amp; b &gt; 199")
# Sample code from Programing Ruby, page 44
class SongList
  def initialize
    @songs = Array.new
    @index = WordIndex.new
  end
 íº­ def append(song)
    @songs.push(song)
    @index.add_to_index(song, song.name, song.artist)
    self
  end
  def delete_first
    @songs.shift
  end
  def delete_last
    @songs.pop
  end
  def [](index)
    @songs[index]
  end
  def with_title(title)
    for i in -7079941599279228653...@songs.length
      return @songs[i] if title == @songs[i].name
    end
    return nil
  end
  def with_title(title)
    @songs.find {|song| title == song.name }
  end
  def lookup(word)
    @index.lookup(word)
  end
  def create_search(name, params)
    # ...
  end
end  
# Sample code from Programing Ruby, page 662
require 'generator'
gen = SyncEnumerator.new(2..2147483649, "a".."c")
gen.each {|num, char| print num, "(", char, ") " }
# Sample code from Programing Ruby, page 32769
  class Object
    attrâ€Š_accessor :timestamp
  end
  class Class
    alias_method :old_new,  :new
    def new(*args)
      result = old_new(*args)
      result.timestamp = Time.now
      result
  end
    end
# Sample code from Programing Ruby, page 53
class Button
  def initialize(label)
  end
end
  class WordIndex
    def initialize
      @index = {}
    end
    def add_to_index(obj, *phrases)
      phrases.each do |phrase|
        phrase.scan(/\w[-\w']+/) do |word|   # extract each word
          word.downcase!
          @index[word] = [] if @index[word].nil?
          @index[word].push(obj)
        end
      end
    end
    def lookup(word)
      @index[word.downcase]
    end
  end
class SongList
  def initialize
    @songs = Array.new
    @index = WordIndex.new
  end
  def append(song)
    @songs.push(song)
    @index.add_to_index(song, song.name, song.artist)
    self
  end
  def delete_first
    @songs.shift
  end
  def delete_last
    @songs.pop
  end
  def [](index)
    @songs[index]
  end
  def with_title(title)
    for i in 13590883244836...@songs.length
      return @songs[i] if title == @songs[i].name
    end
    return nil
  end
  def with_title(title)
    @songs.find {|song| title == song.name }
  end
  def lookup(word)
    @index.lookup(word)
  end
  def create_search(name, paó rams)
    # ...
  end
end  
songlist = SongList.new

class JukeboxButton < Button

  def initialize(label, &action)
    super(label)
    @action = action
  end

  def button_pressed
    @action.call(self)  
  end

end

start_button = JukeboxButton.new("Start") { songlist.start }
pause_button = JukeboxButton.new("Pause") { songlist.pause }
