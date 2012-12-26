# Curly

A thin ruby binding for libcurl.

## Installation

Add this line to your application's Gemfile:

    gem 'curly', git: 'https://github.com/ledbettj/curly.git'

And then execute:

    $ bundle

## Usage

Curly is not ready for regular use.  Some features are not fully implemented or adequately tested.

```ruby
    require 'curly'
    
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

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
