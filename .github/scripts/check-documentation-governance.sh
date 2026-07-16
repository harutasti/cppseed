#!/usr/bin/env bash

set -euo pipefail

required_templates=(
  initiative-readme-template.md
  requirements-template.md
  external-design-template.md
  detailed-design-template.md
  adr-template.md
  threat-model-template.md
  test-plan-template.md
  traceability-template.md
  test-report-template.md
  release-runbook-template.md
  acceptance-report-template.md
)

for template in "${required_templates[@]}"; do
  test -f "docs/templates/$template"
done

initiative_count=0
while IFS= read -r -d '' initiative; do
  initiative_count=$((initiative_count + 1))
  register="$initiative/README.md"
  requirements="$initiative/requirements.md"

  test -f "$register"
  test -f "$requirements"

  grep -Eq '^- Initiative ID: `[A-Z][A-Z0-9-]*`$' "$register"
  grep -Eq '^- Overall status: (Proposed|In progress|Blocked|Accepted|Completed|Cancelled)$' \
    "$register"
  grep -Fq '## 3. Roles' "$register"
  grep -Fq '## 4. Gate register' "$register"
  grep -Fq '## 7. Approval record' "$register"
  grep -Fq '## 8. Change history' "$register"

  for gate in $(seq 0 13); do
    grep -Fq "| G$gate |" "$register"
  done

  grep -Eq '^### (FR|SR|NFR)-[A-Z][A-Z0-9-]*-[0-9]{3}:' "$requirements"
  grep -Fq '## 9. 受け入れ条件' "$requirements"
  grep -Fq '## 12. 変更履歴' "$requirements"
  grep -Fq '## 13. 承認' "$requirements"
done < <(find docs/initiatives -mindepth 1 -maxdepth 1 -type d -print0)

test "$initiative_count" -gt 0

echo "documentation governance check passed for $initiative_count initiative(s)"
