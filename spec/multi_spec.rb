require 'spec_helper'
require 'spec_server'
require 'curly'
require 'pry'

describe "Curly::Multi" do

  it "works with multiple requests" do
    r1 = Curly::Request.new("#{TEST_URL}/status-test")
    r2 = Curly::Request.new("#{TEST_URL}/body-test")

    m = Curly::Multi.new

    m.add(r1, r2).run

    r1.response.code.should eq(418)

    JSON.parse(r2.response.body).should eq(
      'value' => 1234
    )

  end
end
