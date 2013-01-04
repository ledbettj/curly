# Curly


A thin ruby binding for libcurl.  See the [RDoc documentation](http://rubydoc.info/github/ledbettj/curly/index) for more information.

## Installation

Add this line to your application's Gemfile:

    gem 'curly', git: 'https://github.com/ledbettj/curly.git'

And then execute:

    $ bundle
    
Note that `curly` provided by rubygems.org is something completely different which appears to have been abandonded.

## Usage

Although it is feature-complete for basic use, only daring individuals are advised to use Curly at this time.


### Simple Interface

```ruby
require 'curly'
require 'json'

response = Curly::Request.post("http://api.example.com/items",
  headers: {
    'X-Example-Api-Key' => 'abcf358a63fc',
    'Content-type'      => 'application/json'
  },
  body: {
    'name'  => 'New Item',
    'price' => 3.50,
  }.to_json
)

raise StandardError.new unless response.status == 200

puts "Server said: #{response.status}: #{JSON.parse(response.body)}"
```

### Multi Interface

```ruby
require 'curly'
require 'json'

r1 = Curly::Request.new("http://api.example.com/items",
  method: :post,
  headers: {
    'X-Example-Api-Key' => 'abcf358a63fc',
    'Content-type'      => 'application/json'
  },
  body: {
    'name'  => 'New Item',
    'price' => 3.50,
  }.to_json
)

r2 = Curly::Request.new("http://api.example.com/items/3",
  method: :get,
  headers: { 'X-Example-Api-Key' => 'abcf358a63fc' }
)

multi = Curly::Multi.new
multi.add(r1, r2).run

[r1, r2].each do |req|
  puts "#{req.url} said: #{req.response.status}: #{JSON.parse(req.response.body)}"
end
```

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
