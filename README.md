# pocas -- Plain Old C Applications and Services

pocas is intended to be a set of libraries for building modern applications
and services in plain old C. So far, it's work in progress.

[![Build Status](https://travis-ci.org/Zirias/pocas.svg?branch=master)](https://travis-ci.org/Zirias/pocas)

## pocascore

The pocascore library contains basic functionality like containers (e.g. a
List and a HashTable), colored text output, a generic eventloop (this should
probably use libevent later), access to files, etc.

## pocastest

This is a test framework and a test runner. The runner loads a "test class"
from a shared object and executes each individual testcase in an isolated
sub-process. pocastest is currently used in the CI for pocas.

## pocasgui

A very lightweight GUI library that doesn't do any drawing but works mostly
as an adapter through backend modules. It's not yet usable, but there are
working backend implementations for the `win32` API and for Qt (version 4 or
5).

