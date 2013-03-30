#!/usr/bin/env ruby

# (c) oblong industries

=begin

  wit-worker

  This is part of the 'hydra' example. Hydra is a graphical
  front-end for two long-running 'worker' programs: imago-worker
  and wit-worker.

  wit-worker listens to the 'witter' pool for any proteins
  having a "wit-request" descrip and an "argument" ingest.
  It makes an HTTP request to Twitter's trends API, and
  puts the result into a new protein, which is deposited
  back into the 'witter' pool.

  Instructions:

  1. run wit-worker as follows, and leave it running:

        ./wit-worker.rb

        OR

        ruby wit-worker.rb

  2. Optionally, open a terminal and 'peek witter' to
     observe the pool traffic on the 'witter' pool.
     This program will create the pool if it doesn't exist.

  2. Run hydra (see examples/hydra/)

  3. When you click the text object in hydra, it puts a
     request protein in the 'witter' pool.  wit-worker
     watches for these proteins, does the Twitter request,
     and puts a new result protein into the pool.


  Notes:

  - wit-worker.rb accepts one optional argument, naming a pool
    to use other than 'witter'.  To use a pool named 'foo':

        ./wit-worker.rb foo

  - To listen to a pool on another machine:

        ./wit-worker.rb tcp://hostname/poolname

  - To create a new pool from the command line:

      p-create witter

=end

require 'rubygems'
$LOAD_PATH.unshift "/opt/oblong/greenhouse/lib/ruby"
require 'Pool'
require 'Protein'
include Plasma
require 'pp'

begin
  require 'rest-client'
rescue LoadError
  puts "The 'rest-client' gem must be installed to run this example."
  puts "Please do 'sudo gem install rest-client' and try again."
  exit
end

begin
  require 'json'
rescue LoadError
  puts "The 'json' gem must be installed to run this example."
  puts "Please do 'sudo gem install json' and try again."
  exit
end


def hose_to(pool)
  Pool.create(pool, {"size" => Plasma::POOL_DEFAULT_SIZE})
rescue Plasma::PoolExistsError
  Pool.participate pool
rescue
  puts "Pool setup error: #{$!.class}: #{$!}"
  nil
end


def slaw_from_twitter()
  url = 'https://api.twitter.com/1/trends/1.json'
  response = RestClient.get url

  j = JSON.parse(response.body)
  #puts "\nJSON.parse =\n\n"
  #puts j.inspect

  Slaw.new j
end


$poolname = 'witter'
$poolname = ARGV[0] if ARGV[0] && ARGV[0].length > 0
hose = hose_to $poolname
if ! hose
  puts "wit-worker couldn't open a hose to the '#{$poolname}' pool."
  exit
end


puts "wit-worker is watching the '#{$poolname}' pool."
puts "Listening for any proteins with descrip 'wit-request' & ingest 'argument'"
while true
  # If there's a protein available on the hose, inspect it
  # A side-effect of hose.next() is to advance our hose index by 1
  if prot = hose.next(1)
    descrips = prot.descrips.to_ary
    ingests  = prot.ingests.to_hash
    if (descrips.include?("wit-request") &&
        ingests.include?("argument"))

      puts "wit-request protein received: " + descrips.inspect +
           ", " + ingests.inspect
      puts "making twitter request..."

      s = slaw_from_twitter()

      # Let's digest that slaw a bit; we got a list, want the first element.
      # Within that list is a map, from which we want the 'trends'
      # key/value pair
      s = s[0]["trends"]

      # Have a look-see
      # pp s.class    # It's an array
      # pp s

      puts "...and depositing result in the pool."

      hose.deposit Protein.new(["wit-result"],
                               {"trends", s})
    end
  end
end
