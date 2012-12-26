require 'spec_helper'

describe "Curly::Parameterize" do

  let(:test) {
    {
      :x => 'a',
      :b => [1, 2, 3],
      :z => ['a', 'b', ['derp', 'doodle']],
      'nested' => {
        'first' => 'hello',
        'then'  => 'now',
        'again' => {
          :z => 3,
          :f => ['j', 'jk', 'jlk']
        }
      }
    }
  }

  it "behaves the same as to_query when unescaped" do
    require 'active_support/core_ext/object/to_query'

    support = CGI.unescape(test.to_query)
    ours    = CGI.unescape(Curly::Parameterize.query_string(test))

    support.should eq(ours)
  end

  it "behaves the same as to_query when escaped" do
    require 'active_support/core_ext/object/to_query'

    support = test.to_query
    ours    = Curly::Parameterize.query_string(test)

    support.should eq(ours)
  end
end
