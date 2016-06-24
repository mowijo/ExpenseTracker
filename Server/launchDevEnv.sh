#!/bin/bash
SF="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

$SF/launchMysql.sh
$SF/launchApache.sh