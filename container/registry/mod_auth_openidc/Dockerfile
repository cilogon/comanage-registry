# COmanage Registry Dockerfile template
#
# Portions licensed to the University Corporation for Advanced Internet
# Development, Inc. ("UCAID") under one or more contributor license agreements.
# See the NOTICE file distributed with this work for additional information
# regarding copyright ownership.
#
# UCAID licenses this file to you under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with the
# License. You may obtain a copy of the License at:
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
ARG COMANAGE_REGISTRY_VERSION=develop
ARG COMANAGE_REGISTRY_BASE_IMAGE_VERSION=1
ARG MOD_AUTH_OPENIDC_VERSION

FROM comanage-registry-base:${COMANAGE_REGISTRY_VERSION}-${COMANAGE_REGISTRY_BASE_IMAGE_VERSION} AS comanage

FROM debian:bookworm-slim AS building

ARG MOD_AUTH_OPENIDC_VERSION
ENV MOD_AUTH_OPENIDC_VERSION=${MOD_AUTH_OPENIDC_VERSION:-2.4.12.2}

RUN apt-get update \
      && apt-get install -y --no-install-recommends \
        apache2 \
        apache2-dev \
        automake \
        ca-certificates \
        libcjose0 \
        libcjose-dev \
        libcurl4-openssl-dev \
        libjansson-dev \
        libpcre3-dev \
        libssl-dev \
        pkg-config \
        wget 

RUN MOD_AUTH_OPENIDC_URL="https://github.com/zmartzone/mod_auth_openidc/releases/download/v${MOD_AUTH_OPENIDC_VERSION}/mod_auth_openidc-${MOD_AUTH_OPENIDC_VERSION}.tar.gz" \
      && mkdir -p /tmp/mod_auth_openidc \
      && wget -O mod_auth_openidc.tar.gz "${MOD_AUTH_OPENIDC_URL}" \
      && tar -zxf mod_auth_openidc.tar.gz -C /tmp/mod_auth_openidc --strip-components=1 \
      && cd /tmp/mod_auth_openidc \
      && ./configure --with-apxs2=`which apxs2` \
      && make \
      && make install 

FROM comanage

ARG COMANAGE_REGISTRY_BASE_IMAGE_VERSION
ENV COMANAGE_REGISTRY_BASE_IMAGE_VERSION=${COMANAGE_REGISTRY_BASE_IMAGE_VERSION}
LABEL comanage_registry_base_image_version=${COMANAGE_REGISTRY_BASE_IMAGE_VERSION}

ARG MOD_AUTH_OPENIDC_VERSION
ENV MOD_AUTH_OPENIDC_VERSION=${MOD_AUTH_OPENIDC_VERSION:-2.4.10}
LABEL mod_auth_openidc_version=${MOD_AUTH_OPENIDC_VERSION}

RUN apt-get update \
        && apt-get install -y --no-install-recommends \
          libcjose0

COPY --from=building /usr/lib/apache2/modules/mod_auth_openidc.so /usr/lib/apache2/modules/mod_auth_openidc.so

RUN echo "LoadModule auth_openidc_module /usr/lib/apache2/modules/mod_auth_openidc.so" > /etc/apache2/mods-available/auth_openidc.load \
      && a2enmod auth_openidc 

COPY container/registry/mod_auth_openidc/htaccess /srv/comanage-registry/app/webroot/.htaccess

# Deploy the CILogon ePPN enroller plugin
ARG CILOGON_EPPN_ENROLLER_PLUGIN_SRC_URL=https://github.com/cilogon/CilogonEppnEnroller/archive/main.tar.gz

RUN mkdir -p "$COMANAGE_REGISTRY_DIR/app/Plugin/CilogonEppnEnroller" \
      && wget -O CilogonEppnEnroller.tar.gz $CILOGON_EPPN_ENROLLER_PLUGIN_SRC_URL \
      && tar -zxf CilogonEppnEnroller.tar.gz -C "$COMANAGE_REGISTRY_DIR/app/Plugin/CilogonEppnEnroller" --strip-components=1 \
      && rm -f CilogonEppnEnroller.tar.gz \
      && cd "$COMANAGE_REGISTRY_DIR/app/Plugin/CilogonEppnEnroller/View/CilogonEppnEnrollers" \
      && ln -s ../../../../View/Standard/edit.ctp edit.ctp

# Deploy Oa4mpClient plugin version 6.0.0
ARG CILOGON_OA4MP_CLIENT_PLUGIN_SRC_URL=https://github.com/cilogon/Oa4mpClient/archive/6.0.0.tar.gz

RUN mkdir -p "$COMANAGE_REGISTRY_DIR/app/Plugin/Oa4mpClient" \
      && wget -O Oa4mpClient.tar.gz $CILOGON_OA4MP_CLIENT_PLUGIN_SRC_URL \
      && tar -zxf Oa4mpClient.tar.gz -C "$COMANAGE_REGISTRY_DIR/app/Plugin/Oa4mpClient" --strip-components=1 \
      && rm -f Oa4mpClient.tar.gz

# Copy customized core.php that sets a session timeout to 8 hours
COPY container/registry/mod_auth_openidc/core.php "$COMANAGE_REGISTRY_DIR/app/Config/"

# Install MIT Kerberos runtime libraries and development files.
RUN apt-get update && apt-get install -y \
        libkrb5-3 \
        libkrb5-dev

# Default PECL krb5 version but may be overriden using a build argument.
ARG PECL_KRB5_VERSION=1.1.4
ENV PECL_KRB5_VERSION=${PECL_KRB5_VERSION}

# Use the helper functions from the upstream php Docker image
# to build, install, and enable the PECL krb5 module. After
# build is complete purge the development libraries and source.
RUN docker-php-source extract \
        && pecl bundle -d /usr/src/php/ext krb5-${PECL_KRB5_VERSION} \
        && docker-php-ext-configure krb5 --with-krb5kadm \
        && docker-php-ext-install krb5 \
        && docker-php-ext-enable krb5 \
        && docker-php-source delete \
        && apt-get purge -y libkrb5-dev \
        && apt-get clean

# Allow values for first administrator bootstrapped into the
# platform to be specified at image build time, in addition to
# being injected at run time through the entrypoint script.
ARG COMANAGE_REGISTRY_ADMIN_GIVEN_NAME
ARG COMANAGE_REGISTRY_ADMIN_FAMILY_NAME
ARG COMANAGE_REGISTRY_ADMIN_USERNAME
ARG COMANAGE_REGISTRY_ENABLE_POOLING

# Set simple defaults for first administrator bootstrapped into the
# platform to make simple evaluation of the platform easier.
ENV COMANAGE_REGISTRY_ADMIN_GIVEN_NAME=${COMANAGE_REGISTRY_ADMIN_GIVEN_NAME:-Registry}
ENV COMANAGE_REGISTRY_ADMIN_FAMILY_NAME=${COMANAGE_REGISTRY_ADMIN_FAMILY_NAME:-Admin}
ENV COMANAGE_REGISTRY_ADMIN_USERNAME=${COMANAGE_REGISTRY_ADMIN_USERNAME:-registry.admin}
ENV COMANAGE_REGISTRY_ENABLE_POOLING=${COMANAGE_REGISTRY_ENABLE_POOLING:-No}
