for i in (1..4)
print i," "
end
print "\n"

for i in (1...4)
print i," "
end
print "\n"

items = [ 'Mark', 12, 'goobers', 18.45 ]
for it in items
print it, " "
end
print "\n"

for i in (0...items.length)
print items[0..i].join(" "), "\n"
end
