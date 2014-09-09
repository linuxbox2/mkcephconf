{{# base_section}}
;
; {{title}} ceph.conf file.
;
; This file defines cluster membership, the various locations
; that Ceph stores data, and any other runtime options.

; If a 'host' is defined for a daemon, the start/stop script will
; verify that it matches the hostname (or else ignore it).  If it is
; not defined, it is assumed that the daemon is intended to start on
; the current host (e.g., in a setup with a startup.conf on each
; node).

[global]
	admin socket = {{var-run-ceph}}/ceph-$name.$id.asok

        ; enable secure authentication
        auth supported = none

	auth cluster required = none
	auth service required = none
	auth client required = none

        ; allow ourselves to open a lot of files
        max open files = {{max-open-files}}

        ; set up logging
        log file = {{var-log-ceph}}/$name.log

        ; set up pid files
        pid file = {{var-run-ceph}}/$name.pid

	ms headercrc = false
	ms datacrc = false

	; valid for any size?
	osd pool default size = 2
	osd pool default min size = 1
	osd crush chooseleaf type = 0

	; prod debug
	debug client = 20
	debug monclient = 11
	debug xio = 0/0
	debug ms = 11
	debug cls  = 11
	debug rbd = 11
	debug filestore = 11

	xio queue depth = {{xio-queue-depth}}
	xio port shift = {{xio-port-shift}}
        rdma local = {{rdma-bind-addr}}
	cluster rdma = true
	client rdma = true

; monitors
;  You need at least one.  You need at least three if you want to
;  tolerate any node failures.  Always create an odd number.
[mon]
        mon data = {{ceph-data}}/mon$id

        ; logging, for debugging monitor crashes, in order of
        ; their likelihood of being helpful
        ;debug ms = 0/0
	;debug mon = 20
        debug mon = 20
        ;debug paxos = 20
        debug auth = 0/0

[mon.0]
        host = {{mon-0-host}}
        mon addr = {{mon-0-addr}}:{{mon-0-port}}
	user = root

; mds
;  You need at least one.  Define two to get a standby.
[mds]
        ; where the mds keeps it's secret encryption keys
        keyring = {{ceph-data}}/keyring.$name

        ; mds logging to debug issues.
        ;debug ms = 0/0
        ;debug mds = 20
	debug mds = 4

	; if port is explicitly set, blacklisting becomes
	; a problem--but otherwise, Xio code incorrectly uses
	; port 0
	public addr = {{mds-0-addr}}:{{mds-0-port}}

	objecter timeout = 10
	mds reconnect timeout = 5
	mds beacon interval = 2

[mds.0]
        host = {{mds-0-host}}
	user = root

; osd
;  You need at least one.  Two if you want data to be replicated.
;  Define as many as you like.
[osd]
        ; This is where volumes will be mounted.
        osd data = {{ceph-data}}/osd$id

        osd journal = {{ceph-data}}/osd$id/journal
        osd journal size = {{osd-journal-size}} ; journal size, in megabytes

	osd class dir = {{ceph-dir}}/lib/rados-classes

        osd mkfs type = {{osd-fstype}}
	osd mount options {{osd-fstype}} = rw,noatime

	osd scrub load threshold = 5

        ; osd logging to debug osd issues, in order of likelihood of being
        ; helpful
 	;debug ms = 0/0
        debug osd = 20
        ;debug filestore = 0/0
        ;debug journal = 20

{{/ base_section}}

{{# osds}}
[osd.{{osd-ix}}]
        host = {{osd-host}}
	user = root

	; aieee (need this until sanitized binding merged)
	ms bind port min = {{osd-bind-port-min}}
	ms bind port max = {{{osd-bind-port-max}}

	devs = {{osd-dev}}
	{{nl}}
{{/ osds}}
