## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

import os
import ns3waf


def configure(conf):
    ns3waf.check_modules(conf, ['core', 'network', 'internet'], mandatory = True)
    ns3waf.check_modules(conf, ['point-to-point', 'tap-bridge', 'netanim'], mandatory = False)
    ns3waf.check_modules(conf, ['wifi', 'point-to-point', 'csma', 'mobility'], mandatory = False)
    ns3waf.check_modules(conf, ['point-to-point-layout'], mandatory = False)
    ns3waf.check_modules(conf, ['topology-read', 'internet-apps', 'applications', 'visualizer'], mandatory = False)




def build(bld):
    bld.build_a_script('dce', needed = ['core', 'internet', 'dce-quagga', 'point-to-point', 'netanim', 'applications' ],
                                  target='bin/project-bgpd',
                                  source=['project-bgpd.cc'],
                                  )

