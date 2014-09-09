#!/bin/bash

# mk datadir
{{{mkdatadir}}}

# unmount all
{{# umount}}
{{{umount_cmd}}}
{{nl}}
{{/ umount}}

# mktmpfs and backing
{{# mktmpfs}}
{{{mkdir}}}
{{{mount}}}
{{{dd}}}
{{nl}}
{{/ mktmpfs}}
